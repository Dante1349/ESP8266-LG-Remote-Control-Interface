/*
A90   on/off
A50   input

090   prog+
890   prog-

490   vol+
C90   vol-

2F0   up
2D0   left
CD0   right
AF0   down
a70   ok
62E9  return

6D25  guide


*/

// ir driver
// https://i.stack.imgur.com/DIM0T.png

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <FS.h>
#include <LittleFS.h>

void handleFileRequest();

String getContentType(String filename);

uint64_t convertStringToU64(String val);

String u64ToString(uint64_t val);

void sendJsonTransmissionResponse(String code, String error);

void sendJsonReceiveResponse(String code, String type);

void readCode();

void sendHex();

int countBits(String val);

ESP8266WebServer server(80);

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
const uint16_t kRecvPin = 2;

// ESP8266 GPIO pin to use. Recommended: 4 (D2).
const uint16_t kIrLed = 0;


IRrecv irrecv(kRecvPin);
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.


decode_results results;

void handleFileRequest() {
    String path = server.uri();
    Serial.println(path);
    if (path.endsWith("/")) { path += "index.html"; }

    if (LittleFS.exists(path)) {
        File file = LittleFS.open(path, "r");
        server.streamFile(file, getContentType(path)); // Get content type dynamically
        file.close();
    } else {
        server.send(404, "text/plain", "File Not Found");
    }
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return "image/jpeg";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    // Initialize LittleFS
    if(!LittleFS.begin()){
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();

    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("IR-Server-AP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    // wait for wifi
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    WiFi.hostname("IR-Server");
    Serial.println("WiFi connected");

    // Print the IP address
    Serial.println(WiFi.localIP());

    server.on("/readCode", readCode);
    server.on("/send", sendHex);
    server.onNotFound(handleFileRequest);

    // Start the server
    server.begin();
    Serial.println("Web Server started");

    irsend.begin();
    irrecv.enableIRIn();
    while (!Serial)  // Wait for the serial connection to be established.
        delay(50);
    Serial.print("IR Receiver/Sender started");
}

void loop() {
    server.handleClient();
}

void sendHex() {
    String code = "";
    String frequency = "";
    String error = "";

    if (server.arg("nec") != "") {
        code = server.arg("nec");

        uint64_t codeAsHex = convertStringToU64(code);

        serialPrintUint64(codeAsHex, HEX);
        Serial.println("");

        irsend.sendNEC(codeAsHex);
        sendJsonTransmissionResponse(code, "");
    } else if (server.arg("sony") != "") {
        code = server.arg("sony");
        uint64_t codeAsHex = convertStringToU64(code);

        serialPrintUint64(codeAsHex, HEX);
        Serial.println("");

        int cBits = countBits(code);
        if (cBits < 12) {
            irsend.sendSony(codeAsHex, 12, 1);
        } else {
            irsend.sendSony(codeAsHex, cBits, 1);
        }

        sendJsonTransmissionResponse(code, "");
    } else if (server.arg("raw") != "" && server.arg("freq") != "") {
        code = server.arg("raw");
        frequency = server.arg("freq");

        uint64_t codeAsHex = convertStringToU64(code);

        serialPrintUint64(codeAsHex, HEX);
        Serial.println("");

        // TODO: void IRsend::sendRaw(const uint16_t*, uint16_t, uint16_t)
        //irsend.sendRaw(codeAsHex, countBits(code), convertStringToU64(frequency));

    } else {
        error = "no parameters found!";
        sendJsonTransmissionResponse("", error);
    }
}

void readCode() {
    String code = "";
    String type = "";
    if (irrecv.decode(&results)) {
        // print() & println() can't handle printing long longs. (uint64_t)
        serialPrintUint64(results.value, HEX);
        Serial.println("");
        irrecv.resume();  // Receive the next value

        code = u64ToString(results.value);
        if (code == NULL) {
            code = "";
        }
        type = typeToString(results.decode_type);
        if (type == NULL) {
            type = "";
        }
    }

    sendJsonReceiveResponse(code, type);
}

void sendJsonTransmissionResponse(String code, String error) {
    String json = "{\r\n"
                  + String("  \"code\": \"" + code + "\",\r\n")
                  + String("  \"error\": \"" + error + "\"\r\n")
                  + String("}");

    server.send(200, "text/plain", json);
}

void sendJsonReceiveResponse(String code, String type) {
    String json = "{\r\n"
                  + String("  \"code\": \"" + code + "\",\r\n")
                  + String("  \"type\": \"" + type + "\"\r\n")
                  + String("}");

    server.send(200, "text/plain", json);
}

uint64_t convertStringToU64(String str)   // char * preferred
{
    uint64_t val = 0;

    for (unsigned int i = 0; i < str.length(); i++) {
        val = val * 16;

        if ((str[i] - '0') > 9) {
            switch (str[i]) {
                case 'A':
                    val = val + 0xA;
                    break;
                case 'B':
                    val = val + 0xB;
                    break;
                case 'C':
                    val = val + 0xC;
                    break;
                case 'D':
                    val = val + 0xD;
                    break;
                case 'E':
                    val = val + 0xE;
                    break;
                case 'F':
                    val = val + 0xF;
                    break;
            }
        } else {
            val = val + str[i] - '0';  // convert char to numeric value
        }
    }
    return val;
}

String u64ToString(uint64_t val) {
    unsigned long long1 = (unsigned long) ((val & 0xFFFF0000) >> 16);
    unsigned long long2 = (unsigned long) ((val & 0x0000FFFF));

    String hex = String(long1, HEX) + String(long2, HEX); // six octets
    return hex;
}

int countBits(String val) { // Should work with any size integer.
    int bits = 0;
    char c = val[0];
    switch (c) {
        case '0':
            bits = 1;
            break;
        case '1':
            bits = 1;
            break;
        case '2':
            bits = 2;
            break;
        case '3':
            bits = 2;
            break;
        case '4':
            bits = 3;
            break;
        case '5':
            bits = 3;
            break;
        case '6':
            bits = 3;
            break;
        case '7':
            bits = 3;
            break;
        case '8':
            bits = 4;
            break;
        case '9':
            bits = 4;
            break;
        case 'A':
            bits = 4;
            break;
        case 'B':
            bits = 4;
            break;
        case 'C':
            bits = 4;
            break;
        case 'D':
            bits = 4;
            break;
        case 'E':
            bits = 4;
            break;
        case 'F':
            bits = 4;
            break;
    }
    return bits + ((val.length() - 1) * 4);
}

