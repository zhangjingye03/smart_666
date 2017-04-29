#include <Arduino.h>
#include "snake.h"
#include <EEPROM.h>
#include "66tb.h"
#include "birthday.h"
#include <Wire.h>
#include <RTClib.h>
RTC_DS3231 rtc;
#define AUDIO_CONTROL 4
#define LATENCYBASE 13107 // 65535 / 5

bool autoC = true;
bool noAuto = false;
bool atMain = true;
unsigned int mainLoop = 0; unsigned char countLoop = 60;
int w = 4;
char showGK = -1; bool scr = false; unsigned int scrLoop = 0;
bool out = true;
bool showTime = true;
snake *sp = NULL; char dir = 4;
int qn = -1, qa;
char syspeed = 5;
DateTime nextExam; int nextExamNum = 0; int nextExamYear, nextExamMonth, nextExamDay;


void setup() {
  float temp;
  // D2 for screen power
  pinMode(2,OUTPUT);
  digitalWrite(2, HIGH);
  /*Serial.write(26);
  Serial.println("SPG(0);");*/

  // LOGO
  delay(1321);
  Serial.begin(115200);
  Serial.println(F("TERM;"));
  delay(100);
  Serial.println(F("\r\n-----------------------------"));
  Serial.println(F("Smart 666 Version 2.22"));
  Serial.println(F("Copyright (C) 2016-2017 ZJY"));
  Serial.println(F("-----------------------------\r\n"));
  EEPROM.get(2, syspeed);
  if (syspeed == 0) syspeed = 5;
  Serial.print(F("[OK] System latency -> ")); Serial.println(syspeed, DEC);
  Serial.println(F("[OK] Serial @ 115200 bps"));
  pinMode(4,OUTPUT);
  pinMode(13,OUTPUT);
  Serial.print(F("[OK] Audio control @ D")); Serial.println(AUDIO_CONTROL);
  delay(200);
  Serial.println(F("\r\n- Initializing I2C driver ..."));
  if (!rtc.begin()) {
    Serial.println(F("[x] Couldn't find RTC.")); noAuto = true; autoC = false;
  } else {
    if (rtc.lostPower()) {
      Serial.println(F("[!] RTC lost power. Resetting..."));
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      /*noAuto = true;*/ autoC = false;
    }
    DateTime now = rtc.now();
    temp = rtc.getTemperature();
    Serial.print(F("[OK] "));Serial.print(now.year(), DEC); Serial.print('/'); Serial.print(now.month(), DEC); Serial.print('/'); Serial.println(now.day(), DEC);
    Serial.print(F("     ")); Serial.print(now.hour(), DEC); Serial.print(':'); Serial.print(now.minute(), DEC); Serial.print(':'); Serial.println(now.second(), DEC);
    Serial.print(F("     Temperature: ")); Serial.print(temp); Serial.print(F(" \'C")); Serial.println();
    // 66tb 24c32
    Serial.print("[OK] AT24C32 with ");
    count = mem.readInt(0x0000);
    Serial.print(count, DEC);
    Serial.println(" questions.");
  }
  delay(1000);
  Serial.write(26);
  delay(100);
  Serial.println(F("SNF(0,0);"));
  delay(500);
  Serial.println(F("SPG(4);"));
  delay(200);
  EEPROM.get(4, nextExamYear); EEPROM.get(6, nextExamMonth); EEPROM.get(8, nextExamDay); EEPROM.get(10, nextExamNum);

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

void printExamName () {
  switch (nextExamNum) {
    case 0: Serial.print("\xC1\xAA\xBF\xBC"); break; //联考
    case 1: Serial.print("\xD4\xC2\xBF\xBC"); break; //月考
    case 2: Serial.print("\xD2\xBB\xC4\xA3"); break; //一模
    case 3: Serial.print("\xB6\xFE\xC4\xA3"); break; //二模
    case 4: Serial.print("\xC8\xFD\xC4\xA3"); break; //三模
    case 5: Serial.print("\xC6\xDA\xD6\xD0\xBF\xBC"); break; //联考
    case 6: Serial.print("\xC6\xDA\xC4\xA9\xBF\xBC"); break; //联考
    case 7: Serial.print("\xB5\xF7\xD1\xD0\xBF\xBC"); break; //联考
    case 8: Serial.print("\xCB\xAE\xC6\xBD\xB2\xE2"); break; //联考
    case 9: Serial.print("\xB9\xD8\xB1\xD5"); //关闭
  }
}

void printTime () {
  DateTime now = rtc.now();
  Serial.print(F("SBC(59);DS12(127,208,'"));
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
      sscanf(ttl.c_str(), "[AT:%d]OK", &w);
      if (sscanf(ttl.c_str(), "[BN:%d]\r\n", &bn) == 1) {
        delay(100);
        scrLoop = 0; scr = false;
        //if (bn == 32) atMain = true; else atMain = false;
        if (w == 5) { // ctrl
          if (bn == 1) {
            autoC = false; on();
          } else if (bn == 2) {
            autoC = false; off();
          } else if (bn == 3) {
            autoC = !autoC;
          }
        } else if (w == 144) { // fake 66tb
          if (bn == 7) {
            if (++qn >= count) qn = 0;
            qa = readQuestion(qn);
          } else if (bn == 6) {
            if (--qn < 0) qn = count - 1;
            qa = readQuestion(qn);
          } else if (bn == 5) w = 4; // exit
          else {
            if (bn - 1 == qa) Serial.println(F("SBC(54);DS24(0,0,'Correct',16);")); else Serial.println(F("SBC(53);DS24(0,0,'Wrong',16);"));
            //Serial.print(F("DS24(2,2,'")); Serial.print(tellMeAns(qa)); Serial.println(F("',16);"));
          }

        } else if (w == 12) { // snake
          if (!sp && bn != 1 /* 1 == exit */) { sp = new snake(); Serial.println(F("BOXF(8,2,166,168,47);")); /*clear screen*/ }
          if (bn == 2) dir = 3;
          else if (bn == 3) dir = 1;
          else if (bn == 4) dir = 2;
          else if (bn == 5) dir = 4;
          else if (bn == 1) { delete sp; dir = 4; sp = NULL; w = 0; /* fake a 'w' else it will create a new snake! */}
        } else if (w == 11) { // rtc
          DateTime now = rtc.now(), adj = now;
          if (bn == 7) showTime = !showTime; // switch datetime
          else if (bn == 1) { // up1
            if (showTime) { // hh
              adj = DateTime(now + TimeSpan(3600));
            } else { // YYYY
              adj = DateTime(now + TimeSpan(365, 0, 0, 0));
            }
          } else if (bn == 2) { // down1
            if (showTime) { // hh
              adj = DateTime(now - TimeSpan(3600));
            } else { // YYYY
              adj = DateTime(now - TimeSpan(365, 0, 0, 0));
            }
          } else if (bn == 3) { // up2
            if (showTime) { // mm
              adj = DateTime(now + TimeSpan(60));
            } else { // MM
              adj = DateTime(now + TimeSpan(30, 0, 0, 0));
            }
          } else if (bn == 4) { // down2
            if (showTime) { // mm
              adj = DateTime(now - TimeSpan(60));
            } else { // MM
              adj = DateTime(now - TimeSpan(30, 0, 0, 0));
            }
          } else if (bn == 5) { // up3
            if (showTime) { // ss
              adj = DateTime(now + TimeSpan(1)); // +1s
            } else { // DD
              adj = DateTime(now + TimeSpan(1, 0, 0, 0));
            }
          } else if (bn == 6) { // down3
            if (showTime) { // ss
              adj = DateTime(now - TimeSpan(1));
            } else { // DD
              adj = DateTime(now - TimeSpan(1, 0, 0, 0));
            }
          }
          if (now.unixtime() != adj.unixtime()) rtc.adjust(adj); // Not set if not changed
        } else if (w == 6) { // count
          //if (bn == 30 || bn == 29) {
          if (noAuto)
            Serial.println("DS12(0,0,'RTC\xC4\xA3\xBF\xE9\xB9\xCA\xD5\xCF',1);"); //DS12(0,0,'RTC模块故障',1);
          else
            countLoop = 60;
        } else if (w == 4) { // main page
          if (bn == 4) sp = new snake();
          else if (bn == 2) {
            showGK = -1; countLoop = 60; w = 6;
          }
        } else if (w == 10) { // adv
          if (bn == 3) {
            if (++syspeed > 5) syspeed = 1;
            EEPROM.put(2, syspeed);
          }
        } else if (w == 166) { // examset fake
          if (bn == 1) {
            nextExam = DateTime(nextExam + TimeSpan(365, 0, 0, 0));
          } else if (bn == 2) {
            nextExam = DateTime(nextExam - TimeSpan(365, 0, 0, 0));
          } else if (bn == 3) {
            nextExam = DateTime(nextExam + TimeSpan(30, 0, 0, 0));
          } else if (bn == 4) {
            nextExam = DateTime(nextExam - TimeSpan(30, 0, 0, 0));
          } else if (bn == 5) {
            nextExam = DateTime(nextExam + TimeSpan(1, 0, 0, 0));
          } else if (bn == 6) {
            nextExam = DateTime(nextExam - TimeSpan(1, 0, 0, 0));
          } else if (bn >= 10 && bn <= 19) {
            nextExamNum = bn - 10;
            Serial.print(F("SBC(0);DS12(0,0,'\xC9\xE8\xD6\xC3\xD2\xD1\xB1\xA3\xB4\xE6\xA3\xBA',16);DS12(0,13,'")); // 设置已保存：
            if (nextExamNum != 9) { // 9 == no exam
              nextExamYear = nextExam.year();
              nextExamMonth = nextExam.month();
              nextExamDay = nextExam.day();
              EEPROM.put(4, nextExamYear);
              EEPROM.put(6, nextExamMonth);
              EEPROM.put(8, nextExamDay);
            }
            EEPROM.put(10, nextExamNum);
            Serial.print(nextExam.year()); Serial.print(nextExam.month()); Serial.print(nextExam.day()); printExamName();
            Serial.println(F("',16,0);"));
            delay(1000); Serial.println(F("SPG(10);"));
          }
        }


      }
    }
    ttl = "";
  }

// not always run code
if (++mainLoop == syspeed * LATENCYBASE) {
  mainLoop = 0;
  if ( w != 6 && ( (++scrLoop > 60 && w != 144) || (scrLoop > 1000 && w == 144) )) { // When doing exercise, extend the delay of scr
  //if (++scrLoop > 60 && w != 6) {
    scrLoop = 0; scr = true; w = 6; return;
  }
  // Judge where I am
  if (w == 4) { // at SPG4, main menu, need to refresh temp
    Serial.print(F("SBC(53);LABL(24,1,179,86,'")); Serial.print(rtc.getTemperature(),1); Serial.print(F(" \\'C',16,1);")); printTime();
  }

  if (w == 12) {
    if (!sp) return;
    syspeed = 3;
    if (sp->hasFood == 0 && random(2) == 0) {
      sp->fx = random(SIZE); sp->fy = random(SIZE); sp->hasFood = 1;
    }
    char res = sp->moveSnake(dir);
    sp->printSnake();
    if (res == 1 || res == 3) {
      int highest = 0; EEPROM.get(0, highest);
      if (highest >= sp->score) {
        Serial.print(F("SBC(47);LABL(12,8,163,166,'\xC0\xAC\xBB\xF8, \xD7\xEE\xB8\xDF\xB7\xD6"));
        Serial.print(highest, DEC);
        Serial.print(F(", \xC4\xE3\xB2\xC5"));
        Serial.print(sp->score, DEC);
        Serial.println(F("',15,1);"));
      } else {
        EEPROM.put(0, sp->score);
        Serial.print(F("SBC(47);LABL(12,8,163,166,'\xCD\xDB! "));
        Serial.print(sp->score, DEC);
        Serial.println(F("! \xC4\xE3\xB4\xF2\xC6\xC6\xC1\xCB\xBC\xC7\xC2\xBC\xA3\xA1',15,1"));
      }
      delete sp; sp = NULL;
    } else if (res == 2) {
      Serial.println(F("SBC(47);LABL(12,8,163,166,'\xD4\xDD\xCD\xA3',15,1);"));
    } else if (res == 4) {
      EEPROM.put(0, sp->score);
      Serial.println(F("SSBC(47);LABL(12,8,163,166,'\xC4\xE3\xD3\xAE\xC1\xCB',15,1);"));
      delete sp; sp = NULL;
    } else {
      Serial.print(F("SBC(47);LABL(12,8,163,166,'Score:  ")); Serial.print(sp->score, DEC); Serial.println(F("',15,1);"));
    }
  } else {
    delete sp; sp = NULL; EEPROM.get(2,syspeed);
  }

  if (w == 14) { // 66tb
    qn = /*(qn == -1) ? */random(count) /*: qn*/;
    verYear = mem.readInt(0x0002); verDate = mem.readInt(0x0004);
    if (mem.readInt(0x0000) == 0) Serial.println("DS16(8,40,'\xB3\xF5\xCA\xBC\xBB\xAF\xCA\xA7\xB0\xDC',16);");
    else qa = readQuestion(qn);
    w = 144; // fake
  }

  if (w == 6) { // at SPG6, countDown, need to refresh display mode
      if (countLoop >= 60) {
        countLoop = 0;
        showGK++;
        DateTime now = rtc.now();
        char tobir = getTodayBir(now.month(), now.day());
        if (showGK > 2) {
          if (scr) { // screensaver mode, just set to 0
            showGK = 0;
          } else { // not screensaver, return to main
            showGK = -1;
            Serial.println(F("SPG(4);")); delay(100);
            return;
          }
        }
        if (showGK == 0 || showGK == 1) {
          unsigned int td;
          if (showGK == 1 || nextExamNum == 9) {
            td = dayDiff(now.year(), now.month(), now.day(), (nextExamMonth >= 7) ? (nextExamYear + 1) : nextExamYear, 6, 7);
          } else {
            td = dayDiff(now.year(), now.month(), now.day(), nextExamYear, nextExamMonth, nextExamDay);
          }
          if (td < 0) td = 0;
          char d1 = td / 100; char d2 = (td - d1 * 100) / 10; char d3 = (td - d1 * 100 - d2 * 10);
          if (d1 == 0) d1 = 10;
          if (d2 == 0) d2 = 10;
          if (d3 == 0) d3 = 10;
          Serial.println(F("SPG(6);")); delay(50); Serial.print(F("LABL(24,0,8,175,'\xBE\xE0\xC0\xEB")); //距离
          if (showGK == 1 || nextExamNum == 9) /*高考*/ Serial.print(F("\xB8\xDF\xBF\xBC")); else printExamName();
          Serial.print(F("\xBB\xB9\xD3\xD0',15,1);")); //还有
          Serial.print(F("CPIC(1,104,3,")); Serial.print((d1 - 1) * 60); Serial.print(F(",0,60,81);"));
          Serial.print(F("CPIC(59,104,3,")); Serial.print((d2 - 1) * 60); Serial.print(F(",0,60,81);"));
          Serial.print(F("CPIC(116,104,3,")); Serial.print((d3 - 1) * 60); Serial.print(F(",0,60,81);"));
          if (scr) Serial.print(F("BTN(32,0,0,175,219,0,132);")); else Serial.print(F("BTN(30,0,0,175,219,0);"));
          Serial.println(F("TPN(2);"));
        } else {
          if (tobir == -1) {
            Serial.println(F("CLS(0);")); delay(50); Serial.print(F("BPIC(1,0,0,15);"));
          } else {
            Serial.println(F("SPG(13);")); delay(50); Serial.print(F("PLAB(24,0,0,179,'")); Serial.print(now.year()); Serial.print('.'); Serial.print(now.month(),DEC); Serial.print('.');
            Serial.print(now.day(),DEC); Serial.print(F("',2,1,1);PLAB(48,0,140,179,'")); Serial.print(birName[tobir]); Serial.print(F("',"));
            Serial.print(/*isboy?*/(birSex[tobir] == 0) ? 4 : 6); Serial.print(F(",1,1);PLAB(24,0,190,179,'"));
            if (tobir == BIRMEN - 1 || tobir == BIRMEN - 2) Serial.print("17"); else Serial.print("18"); // TWT WPF
            Serial.print(F("\xCB\xEA\xC9\xFA\xC8\xD5\xBF\xEC\xC0\xD6',59,1,1);"));
          }
          if (scr) Serial.print(F("BTN(32,0,0,175,219,0,132);")); else Serial.print(F("BTN(30,0,0,175,219,0);"));
          Serial.println(F("TPN(2);"));
        }
      }
      countLoop++;
  }
  if (w == 5) { // ctrl
    if (out) {
      Serial.print(F("SBC(56);DS24(90,15,'\xBF\xAA',6);")); //DS24(51,59,'开',6,0);
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

  if (w == 11) { // rtc
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

  if (w == 10) { //adv
    Serial.print(F("SBC(54);DS24(32,140,'\xD1\xD3\xCA\xB1\xBC\xB6\xB1\xF0 ")); Serial.print(syspeed, DEC);
    Serial.println(F("',0,0);"));
  }

  if (w == 16) { // examset
      nextExam = rtc.now();
      w = 166; // fake
  }

  if (w == 166) { // examset fake
    Serial.print(F("SBC(0);DS32(18,35,'")); Serial.print(nextExam.year()); Serial.print(F("',1,0);DS32(66,35,'/',1,0);DS32(82,35,'"));
    Serial.print(nextExam.month()); Serial.print(F("',1,0);DS32(115,35,'/',1,0);DS32(136,35,'"));
    Serial.print(nextExam.day()); Serial.println(F("',1,0);"));
  }

  // Global auto audio control
  if (autoC) {
    DateTime now = rtc.now();
    char h = now.hour(); char m = now.minute(); char s = now.second();
    int tc = m * 100 + s;
    switch (h) {
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 21: case 23: on(); break;
      case 13: case 19: case 22: off(); break;
      case 8: if (tc >= 3003 && tc <= 3100) off(); else on(); break;
      case 9: if (tc >= 2003 && tc <= 2100) off(); else on(); break;
      case 10: if (tc >= 1003 && tc <= 1100) off(); else on(); break;
      case 11: if (tc >= 1503 && tc <= 1600) off(); else on(); break;
      case 12: if (tc >= 503) off(); else on(); break;
      case 14: if (tc >= 0 && tc <= 2000) off(); else on(); break;
      case 15: if (tc >= 503 && tc <= 600) off(); else on(); break;
      case 16: if ((tc >= 3 && tc <= 100) || (tc >= 5003 && tc <= 5100)) off(); else on(); break;
      case 17: if (tc >= 5003 && tc <= 5100) off(); else on(); break;
      case 18: if (tc >= 5000) off(); else on(); break;
      case 20: if (tc >= 2003 && tc <= 2100) off(); else on(); break;
    }
  }
  int randomize = random(2); // generate a new rand every loop

}
}
