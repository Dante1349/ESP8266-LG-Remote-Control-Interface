import { Component } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterOutlet } from '@angular/router';
import {HttpClient, HttpClientModule} from "@angular/common/http";
import {MatButtonModule} from "@angular/material/button";

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [CommonModule, RouterOutlet, HttpClientModule, MatButtonModule],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
  title = 'remote-control';

  constructor(private http: HttpClient) { }

  public mainButton(val: string) {
    switch (val) {
      case 'onoff':
        this.sendCode('A90');
        break;
      case 'source':
        this.sendCode('A50')
    }
  }

  public numButton(val: number) {
    this.sendCode(this.getNumCode(val));
  }

  public getNumCode(val: number) {
    let code = '';
    switch (val) {
      case 0:
        code = '910';
        break;
      case 1:
        code = '010';
        break;
      case 2:
        code = '810';
        break;
      case 3:
        code = '410';
        break;
      case 4:
        code = 'C10';
        break;
      case 5:
        code = '210';
        break;
      case 6:
        code = 'A10';
        break;
      case 7:
        code = '610';
        break;
      case 8:
        code = 'E10';
        break;
      case 9:
        code = '110';
        break;
    }
    return code;
  }

  public arrowButton(val: string) {
    switch (val) {
      case 'up':
        this.sendCode('2F0');
        break;
      case 'left':
        this.sendCode('2D0');
        break;
      case 'right':
        this.sendCode('CD0');
        break;
      case 'down':
        this.sendCode('AF0');
        break;
      case 'ok':
        this.sendCode('A70');
        break;
      case 'return':
        this.sendCode('62E9');
        break;
    }
  }

  public controlButton(val: string) {
    switch (val) {
      case 'volup':
        this.sendCode('490');
        break;
      case 'voldown':
        this.sendCode('C90');
        break;
      case 'channelup':
        this.sendCode('090');
        break;
      case 'channeldown':
        this.sendCode('890');
        break;
    }
  }

  public additionalButton(val: string) {
    switch (val) {
      case 'guide':
        this.sendCode('6D25');
        break;
      case 'options':
        this.sendCode('36E9');
        break;
    }
  }
  public muteButton() {
    this.sendCode('290');
  }

  public lastChannelButton() {
    this.sendCode('DD0');
  }

  public infoButton() {
    this.sendCode('5D0');
  }

  public homeButton() {
    this.sendCode('070');
  }

  public sendCode(code: string) {
    this.http.get('/send?sony=' + code).subscribe();
  }
}
