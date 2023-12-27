# ESP8266 LG Remote Control Interface
## Parts needed:
* [D1 mini](https://amzn.to/3Rty8ny)
* [D1 mini IR shield](https://www.makershop.de/plattformen/d1-mini/ir-controller-shield/)

## Installation
* install node: https://nodejs.org/en/download
* install @angular/cli: https://angular.io/cli
* install platformio cli: https://docs.platformio.org/en/stable/core/installation/index.html
* Connect D1Mini ord D1MiniPro
* run in terminal "./uploadToD1.sh"

## Usage
* Look for WiFis and Connect to the "IR-Server-AP" Access Point. Enter Your WiFi crendentials. The AP will automatically connect to your WiFi.
* Find the "IR-Server-AP" IP adress in your Router.
* Open a browser and use the ip like following:
    * http://*IP*/
