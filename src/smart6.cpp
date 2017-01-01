#include <Arduino.h>
#include "snake.h"
#include "birthday.h"
#define AUDIO_CONTROL 4

bool autoC = true;
bool noAuto = false;
bool atMain = true;
unsigned int mainLoop = 0; unsigned char countLoop = 60;
bool atCount = false; char showGK = -1; bool scr = false; unsigned char scrLoop = 0;
bool atCtrl = false; bool out = true;
bool atRTC = false; bool showTime = true;
bool atSnake = false; snake *sp = NULL;
char dir = 4;
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <RTClib.h>
RTC_DS3231 rtc;

void setup() {
  float temp;
  // D2 for screen power
  pinMode(2,OUTPUT);
  digitalWrite(2, HIGH);
  /*Serial.write(26);
  Serial.println("SPG(0);");*/

  // LOGO
  delay(1890);
  Serial.begin(115200);
  Serial.println(F("TERM;"));
  delay(100);
  Serial.println(F("\r\n-----------------------------"));
  Serial.println(F("Smart 666 Version 2.0"));
  Serial.println(F("Copyright (C) 2016 ZJY"));
  Serial.println(F("-----------------------------\r\n"));
  Serial.println(F("[OK] Serial @ 115200 bps"));
  pinMode(4,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(3, HIGH); // for emergency backup
  Serial.print(F("[OK] Audio control @ D")); Serial.println(AUDIO_CONTROL);
  delay(200);
  Serial.println(F("\r\n- Initializing I2C driver ..."));
  if (!rtc.begin()) {
    Serial.println(F("[x] Couldn't find RTC.")); noAuto = true; autoC = false;
  } else {
    if (rtc.lostPower()) {
      Serial.println(F("[!] RTC lost power. Resetting..."));
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      noAuto = true; autoC = false;
    }
    DateTime now = rtc.now();
    temp = rtc.getTemperature();
    Serial.print(F("[OK] "));Serial.print(now.year(), DEC); Serial.print('/'); Serial.print(now.month(), DEC); Serial.print('/'); Serial.println(now.day(), DEC);
    Serial.print(F("     ")); Serial.print(now.hour(), DEC); Serial.print(':'); Serial.print(now.minute(), DEC); Serial.print(':'); Serial.println(now.second(), DEC);
    Serial.print(F("     Temperature: ")); Serial.print(temp); Serial.print(F(" \'C")); Serial.println();
  }
  delay(500);
  Serial.write(26);
  delay(100);
  Serial.println(F("SPG(4);"));
  delay(200);
  /*Serial.print(F("DS24(56,179,'\xCE\xC2\xB6\xC8:")); Serial.print(temp,1); Serial.println(F(" \\'C',16,0);"));
  delay(500); atMain = true;*/
}

void on(){
  digitalWrite(AUDIO_CONTROL, LOW);
  out = true;
}
void off(){
  digitalWrite(AUDIO_CONTROL, HIGH);
  out = false;
}

int dayDiff (int Y1, int M1, int D1, int Y2, int M2, int D2) {
  int y2, m2, d2, y1, m1, d1;
  m1 = ( M1 + 9 ) % 12;
  y1 = Y1 - m1 / 10;
  d1 = 365 * y1 + y1 / 4 - y1 / 100 + y1 / 400 + ( m1 * 306 + 5 ) / 10 + ( D1 - 1 );
  m2 = ( M2 + 9 ) % 12;
  y2 = Y2 - m2 / 10;
  d2 = 365 * y2 + y2 / 4 - y2 / 100 + y2 / 400 + ( m2 * 306 + 5 ) / 10 + ( D2 - 1 );
  return (d2 - d1);
}

char getTodayBir (char mon, char day) {
  for (char i = 0; i < BIRMEN; i++) {
    if (mon == birMonth[i] && day == birDay[i]) return i;
  }
  return -1;
}

/*void printV (String in) {
  Serial.print("PS16(0,0,'"); Serial.print(in); Serial.println("',1,0);");
}*/
void printTime () {
  DateTime now = rtc.now();
  Serial.print(F("SBC(53);DS12(127,208,'"));
  if (now.hour() < 10) { Serial.print(0); Serial.print(now.hour()); } else Serial.print(now.hour()); Serial.print(":");
  if (now.minute() < 10) { Serial.print(0); Serial.print(now.minute()); } else Serial.print(now.minute()); Serial.print(":");
  if (now.second() < 10) { Serial.print(0); Serial.print(now.second()); } else Serial.print(now.second()); Serial.println(F("',16,0);")); //STIM(1,2,3);
}

String ttl = "";
void(* resetFunc) (void) = 0;
void loop() {
  if (scr) digitalWrite(13,HIGH); else digitalWrite(13,LOW);
  // Process serial data
  while (Serial.available() > 0){
    ttl += char(Serial.read());
    delay(2);
  }
  if (ttl.length() > 0) {
    if (ttl != "OK") {
      int bn;
      if (sscanf(ttl.c_str(), "[SY:%d]\r\n", &bn) == 1) resetFunc();
      if (sscanf(ttl.c_str(), "[BN:%d]\r\n", &bn) == 1) {      /*ttl != "OK" || ttl != "\r\nOK" || ttl != "\r\nOK\r\n" || ttl != "OK\r\n" || ttl != "OKOK" ) */
        scrLoop = 0; scr = false;
        if (bn == 32) atMain = true; else atMain = false;
        if (bn == 31 || bn == 1 || bn == 2 || bn == 3) {
          atCtrl = true;
          if (bn == 1) {
            autoC = false; on();
          } else if (bn == 2) {
            autoC = false; off();
          } else if (bn == 3) {
            autoC = !autoC;
          }
        } else atCtrl = false;
        if (bn == 27) {
          sp = new snake();
          atSnake = true;
        } else {
          if (atSnake) {
            if (bn == 5) dir = 3;
            else if (bn == 6) dir = 1;
            else if (bn == 7) dir = 2;
            else if (bn == 8) dir = 4;
            else { delete sp; sp = NULL; atSnake = false; }
          }
        }
        if (bn == 25 || bn == 22 || bn == 21 || bn == 20 || bn == 19 || bn == 18 || bn == 17 || bn == 16) {
          atRTC = true;
          DateTime now = rtc.now();
          DateTime adj;
          if (bn == 16 || bn == 25) { // switch datetime, 16
            showTime = !showTime;
          } else {
            if (bn == 22) { // up1
              if (showTime) { // hh
                adj = DateTime(now + TimeSpan(3600));
              } else { // YYYY
                adj = DateTime(now + TimeSpan(365, 0, 0, 0));
              }
            } else if (bn == 21) { // down1
              if (showTime) { // hh
                adj = DateTime(now - TimeSpan(3600));
              } else { // YYYY
                adj = DateTime(now - TimeSpan(365, 0, 0, 0));
              }
            } else if (bn == 20) { // up2
              if (showTime) { // mm
                adj = DateTime(now + TimeSpan(60));
              } else { // MM
                adj = DateTime(now + TimeSpan(30, 0, 0, 0));
              }
            } else if (bn == 19) { // down2
              if (showTime) { // mm
                adj = DateTime(now - TimeSpan(60));
              } else { // MM
                adj = DateTime(now - TimeSpan(30, 0, 0, 0));
              }
            } else if (bn == 18) { // up3
              if (showTime) { // ss
                adj = DateTime(now + TimeSpan(1)); // +1s
              } else { // DD
                adj = DateTime(now + TimeSpan(1, 0, 0, 0));
              }
            } else if (bn == 17) { // down3
              if (showTime) { // ss
                adj = DateTime(now - TimeSpan(1));
              } else { // DD
                adj = DateTime(now - TimeSpan(1, 0, 0, 0));
              }
            }
            rtc.adjust(adj);
          }
        } else {
          if (atRTC) {
            if (bn != 0) atRTC = false;
          }
        }

        if (bn == 30 || bn == 29) {
          if (noAuto) {
            Serial.println("DS12(0,0,'RTC\xC4\xA3\xBF\xE9\xB9\xCA\xD5\xCF',1);"); //DS12(0,0,'RTC模块故障',1);
          } else {
            atCount = true; countLoop = 60;
          }
        } else atCount = false;
        delay(100);
      }
    }
    ttl = "";
  }

// not always run code
if (mainLoop == 65535) {
  if (++scrLoop > 60 && !atSnake && !atCount) {
    scrLoop = 0; atMain = false; atRTC = false; atCtrl = false; atCount = true; scr = true; return;
  }
  // Judge where I am
  if (atMain) { // at SPG4, main menu, need to refresh temp
    Serial.print(F("DS24(56,179,'\xCE\xC2\xB6\xC8:")); Serial.print(rtc.getTemperature(),1); Serial.print(F(" \\'C',16,0);")); printTime();
  }

  if (atSnake) {
    if (!sp) atSnake = false;
    if (sp->hasFood == 0 && random(2) == 0) {
      sp->fx = random(SIZE); sp->fy = random(SIZE); sp->hasFood = 1;
    }
    char res = sp->moveSnake(dir);
    sp->printSnake();
    if (res == 1 || res == 3) {
      Serial.println(F("SBC(0);DS12(0,208,'Game Over!',15);"));
      delete sp; atSnake = false;
    } else if (res == 2) {
      Serial.println(F("SBC(0);DS12(0,208,'Invalid direction, pause.',15);"));
    } else if (res == 4) {
      Serial.println(F("SBC(0);DS12(0,208,'Out of memory. You win.',15);"));
      delete sp; atSnake = false;
    }
  } else {
    delete sp; sp = NULL;
  }


  if (atCount) { // at SPG6, countDown, need to refresh display mode
      if (countLoop >= 60) {
        countLoop = 0;
        showGK++;
        DateTime now = rtc.now();
        char tobir = getTodayBir(now.month(), now.day());
        if ((tobir == -1 && showGK > 2) || (tobir != -1 && showGK > 3)) {
          if (scr) { // screensaver mode, just set to 0
            showGK = 0;
          } else { // not screensaver, return to main
            showGK = -1;
            Serial.println(F("SPG(4);")); delay(200);
            atCount = false; atMain = true; return;
          }
        }
        if (tobir == -1 && showGK == 3) showGK = 0;
        if (showGK == 0 || showGK == 1) {
          unsigned int td;
          if (showGK == 1) {
            td = dayDiff(now.year(), now.month(), now.day(), 2017, 6, 7);
          } else {
            td = dayDiff(now.year(), now.month(), now.day(), 2017, 2, 6);
          }
          byte d1 = td / 100; byte d2 = (td - d1 * 100) / 10; byte d3 = (td - d1 * 100 - d2 * 10);
          if (d1 == 0) d1 = 10;
          if (d2 == 0) d2 = 10;
          if (d3 == 0) d3 = 10;
          Serial.println(F("SPG(6);")); delay(100); Serial.print(F("LABL(24,0,8,175,'\xBE\xE0\xC0\xEB")); Serial.print((showGK) ? F("\xB8\xDF\xBF\xBC") : F("\xBF\xAA\xD1\xA7\xB2\xE2")); Serial.print(F("\xBB\xB9\xD3\xD0',15,1);")); //距离高考/调研考还有
          Serial.print(F("CPIC(1,104,3,")); Serial.print((d1 - 1) * 60); Serial.print(F(",0,60,81);"));
          Serial.print(F("CPIC(59,104,3,")); Serial.print((d2 - 1) * 60); Serial.print(F(",0,60,81);"));
          Serial.print(F("CPIC(116,104,3,")); Serial.print((d3 - 1) * 60); Serial.print(F(",0,60,81);"));
          if (scr) Serial.println(F("BTN(32,0,0,175,219,0,132);")); else Serial.println(F("BTN(30,0,0,175,219,0);"));
        } else if (showGK == 3) {
          Serial.println(F("SPG(13);")); delay(100); Serial.print(F("PLAB(24,0,0,179,'")); Serial.print(now.year()); Serial.print('.'); Serial.print(now.month(),DEC); Serial.print('.');
          Serial.print(now.day(),DEC); Serial.print(F("',2,1,1);PLAB(48,0,140,179,'")); Serial.print(birName[tobir]); Serial.print(F("',6,1,1);PLAB(24,0,190,179,'"));
          if (tobir == BIRMEN - 1 || tobir == BIRMEN - 2) Serial.print("17"); else Serial.print("18"); // TWT WPF
          Serial.print(F("\xCB\xEA\xC9\xFA\xC8\xD5\xBF\xEC\xC0\xD6',59,1,1);"));
          if (scr) Serial.println(F("BTN(32,0,0,175,219,0,132);")); else Serial.println(F("BTN(30,0,0,175,219,0);"));
        } else {
          Serial.print(F("CLS(0);BPIC(1,0,0,15);"));
          if (scr) Serial.println(F("BTN(32,0,0,175,219,0,132);")); else Serial.println(F("BTN(30,0,0,175,219,0);"));
        }
      }
      countLoop++;
  }
  if (atCtrl) {
    if (out) {
      Serial.print(F("SBC(56);DS24(90,15,'\xBF\xAA',47);")); //DS24(51,59,'开',6,0);
      Serial.print(F("SBC(52);DS24(66,70,'\xB9\xD8',15);")); //DS24(51,129,'关',0,0);
    } else {
      Serial.print(F("SBC(56);DS24(90,15,'\xBF\xAA',0);")); //DS24(51,59,'开',15,0);
      Serial.print(F("SBC(52);DS24(66,70,'\xB9\xD8',27);")); //DS24(51,129,'关',2,0);
    }
    if (autoC) {
      Serial.print(F("SBC(54);DS24(66,125,'\xD7\xD4\xB6\xAF\xC4\xA3\xCA\xBD',59);")); //DS16(126,63,'自动模式',4,0);
    } else {
      Serial.print(F("SBC(54);DS24(66,125,'\xD7\xD4\xB6\xAF\xC4\xA3\xCA\xBD',0);")); //DS16(126,63,'自动模式',0,0);
    }
    if (noAuto) {
      Serial.println(F("DS12(0,0,'RTC\xC4\xA3\xBF\xE9\xB9\xCA\xD5\xCF',1);")); //DS12(0,0,'RTC模块故障',1);
    } else {
      printTime();
    }
  }
  if (atRTC) {
    if (noAuto) {
      Serial.println(F("DS12(0,0,'RTC\xC4\xA3\xBF\xE9\xB9\xCA\xD5\xCF',1);")); //DS12(0,0,'RTC模块故障',1);
    } else {
      DateTime now = rtc.now();
      Serial.print(F("SBC(0);CBOF(0,41,220,74,5,0);"));
      if (showTime) {
        Serial.print(F("DS32(18,41,'"));
        if (now.hour() < 10) { Serial.print(0); Serial.print(now.hour()); } else Serial.print(now.hour()); Serial.print(F("',1,0);DS32(66,41,'/',1,0);DS32(82,41,'"));
        if (now.minute() < 10) { Serial.print(0); Serial.print(now.minute()); } else Serial.print(now.minute()); Serial.print(F("',1,0);DS32(115,41,'/',1,0);DS32(136,41,'"));
        if (now.second() < 10) { Serial.print(0); Serial.print(now.second()); } else Serial.print(now.second()); Serial.println(F("',1,0);"));
      } else {
        Serial.print(F("DS32(18,41,'")); Serial.print(now.year()); Serial.print(F("',1,0);DS32(66,41,'/',1,0);DS32(82,41,'"));
        Serial.print(now.month()); Serial.print(F("',1,0);DS32(115,41,'/',1,0);DS32(136,41,'"));
        Serial.print(now.day()); Serial.println(F("',1,0);"));
      }
    }
  }

  // Global auto audio control
  if (autoC) {
    DateTime now = rtc.now();
    byte h = now.hour(); byte m = now.minute(); byte s = now.second();
    int tc = m * 100 + s;
    switch (h) {
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: on(); break;
      case 8: if (tc >= 3003 && tc <= 3100) off(); else on(); break;
      case 9: if (tc >= 2003 && tc <= 2100) off(); else on(); break;
      case 10: if (tc >= 1003 && tc <= 1100) off(); else on(); break;
      case 11: if (tc >= 1503 && tc <= 1600) off(); else on(); break;
      case 12: if ((tc >= 503 && tc <= 600) || tc >= 5500) off(); else on(); break;
      case 13: off(); break;
      case 14: if (tc >= 0 && tc <= 2000) off(); else on(); break;
      case 15: if (tc >= 503 && tc <= 600) off(); else on(); break;
      case 16: if ((tc >= 3 && tc <= 100) || (tc >= 5003 && tc <= 5100)) off(); else on(); break;
      case 17: if (tc >= 1000 && tc <= 2000) off(); else on(); break;
      case 18: if (tc >= 5000) off(); else on(); break;
      case 20: if (tc >= 2003 && tc <= 2100) off(); else on(); break;
      case 19: case 22: off(); break;
      case 21: case 23: on(); break;
    }
  }

}
mainLoop++;
}
