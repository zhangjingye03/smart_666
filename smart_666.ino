#define AUDIO_CONTROL 4
/*#include <IRremote.h>
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;*/
bool autoC = false;
bool noAuto = false;
bool atMain = true;
unsigned int mainLoop = 0; unsigned short countLoop = 60;
bool atCount = false; bool showGK = true;
bool atCtrl = false; bool out = true;
bool atCard = false;

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;
//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// MFRC522
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

void setup() {
  float temp = 1.1;
  // D2 for screen power
  pinMode(2,OUTPUT);
  digitalWrite(2, HIGH);
  /*Serial.write(26);
  Serial.println("SPG(0);");*/
  
  // LOGO
  delay(1500);
  Serial.begin(115200);
  Serial.println("TERM;");
  delay(100);
  Serial.println("Smart 666 made by ZJY");
  Serial.println("Copyright (C) 2016");
  Serial.println("----------------------------------");
  Serial.println("Serial successfully opened @ 115200");
  pinMode(4,OUTPUT);
  // digitalWrite(4, HIGH);
  Serial.print("Configured D"); Serial.print(AUDIO_CONTROL); Serial.print(" for audio control.");
  delay(200);
  Serial.println("Initializing I2C driver ...");
  if (!rtc.begin()) {
    Serial.println("[x] Couldn't find RTC module."); noAuto = true;
  } else {
    if (rtc.lostPower()) {
      Serial.println("[!] RTC lost power. Resetting...");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      noAuto = true;
    }
    DateTime now = rtc.now();
    temp = rtc.getTemperature();
    Serial.print("[OK] "); Serial.print(now.year(), DEC); Serial.print('/'); Serial.print(now.month(), DEC); Serial.print('/');
    Serial.print(now.day(), DEC); Serial.print(" "); Serial.print(now.hour(), DEC); Serial.print(':'); Serial.print(now.minute(), DEC);
    Serial.print(':'); Serial.print(now.second(), DEC); Serial.print(" T: "); Serial.print(temp); Serial.print("C"); Serial.println();
  }
  delay(500);
  Serial.println("Initializing SPI driver ...");
  SPI.begin();        // Init SPI bus
  if (mfrc522.PCD_PerformSelfTest()) { 
    key.keyByte[0] = 0x00;
    key.keyByte[1] = 0x30;
    key.keyByte[2] = 0x03;
    key.keyByte[3] = 0x00;
    key.keyByte[4] = 0x30;
    key.keyByte[5] = 0x03;
    mfrc522.PCD_Init(); // Init MFRC522 card
    Serial.println("[OK] MFRC522 using SPI Bus.");
  } else {
    Serial.println("[x] Unable to config MFRC522.");
  }
  delay(200);
  /*Serial.println("Initializing IRemote driver ...");
  delay(500);
  Serial.println("[!] No IReceiver found.");
  //irrecv.enableIRIn();
  //Serial.println();*/
  //
  delay(1000);
  Serial.write(26);
  delay(100);
  Serial.println("SPG(4);");
  delay(100);
  Serial.print("DS24(80,80,'"); Serial.print(temp); Serial.println("\\'C',0,0);");
  delay(100); atMain = true;
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

float getBnc(byte *buf) {
  return h2d2(buf[4]) * 100 + h2d2(buf[5]) + h2d2(buf[6]) * 0.01;
}

int h2d2 (byte in) {
  return in / 16 * 10 + in % 16;
}

void printV (String in) {
  Serial.print("PS16(0,0,'"); Serial.print(in); Serial.println("',1,0);");
}

String ttl = "";
void loop() {
  // Process serial data
  while (Serial.available() > 0){
    ttl += char(Serial.read());
    delay(2);
  }
  if (ttl.length() > 0) {
    if (ttl == "[BN:1]\r\n") { // manual on, [BN:1]
        autoC = false;
        on(); 
    } else if (ttl == "[BN:2]\r\n") { // manual off, [BN:2]
        autoC = false; 
        off();
    } else if (ttl == "[BN:3]\r\n") { // auto mode, [BN:3]
        autoC = !autoC;
    }
    if (ttl != "OK" /*|| ttl != "\r\nOK" || ttl != "\r\nOK\r\n" || ttl != "OK\r\n" || ttl != "OKOK" */) {
      if (ttl == "[BN:32]\r\n") atMain = true; else atMain = false;
      if (ttl == "[BN:31]\r\n" || ttl == "[BN:1]\r\n" || ttl == "[BN:2]\r\n" || ttl == "[BN:3]\r\n") atCtrl = true; else atCtrl = false;
      if (ttl == "[BN:29]\r\n") atCard = true; else atCard = false;
      if (ttl == "[BN:30]\r\n") {
        if (noAuto) {
          Serial.println("RTC\xC4\xA3\xBF\xE9\xB9\xCA\xD5\xCF"); //RTC模块故障
        } else {
          atCount = true; countLoop = 59;
        }
      } else atCount = false;
      delay(50);
    }
    ttl = "";
  }
  
// not always run code
if (mainLoop == 65535) {
  // Judge where I am
  if (atMain) { // at SPG4, main menu, need to refresh temp
    Serial.print("DS24(80,80,'"); Serial.print(rtc.getTemperature()); Serial.println("\\'C',0,0);");
  }
  if (atCount) { // at SPG6, countDown, need to refresh display mode
      if (countLoop == 60) {
        countLoop = 0; showGK = !showGK;
        DateTime now = rtc.now();
        unsigned int td = 300;
        if (showGK) {
          td = dayDiff(now.year(), now.month(), now.day(), 2017, 6, 7);
        } else {
          td = dayDiff(now.year(), now.month(), now.day(), 2016, 12, 27);
        }
        byte d1 = td / 100; byte d2 = (td - d1 * 100) / 10; byte d3 = (td - d1 * 100 - d2 * 10);
        if (d1 == 0) d1 = 10;
        if (d2 == 0) d2 = 10;
        if (d3 == 0) d3 = 10;
        Serial.print("LABL(24,0,0,220,'\xBE\xE0\xC0\xEB"); Serial.print((showGK) ? "\xB8\xDF\xBF\xBC" : "\xB5\xF7\xD1\xD0\xBF\xBC"); Serial.print("\xBB\xB9\xD3\xD0',15,1);"); //距离高考/调研考还有
        Serial.print("CPIC(12,84,3,"); Serial.print((d1 - 1) * 60); Serial.print(",0,60,81);");
        Serial.print("CPIC(80,84,3,"); Serial.print((d2 - 1) * 60); Serial.print(",0,60,81);");
        Serial.print("CPIC(148,84,3,"); Serial.print((d3 - 1) * 60); Serial.println(",0,60,81);");
      }
      countLoop++;
  }
  if (atCtrl) {
    if (out) {
      Serial.print("SBC(52);DS24(51,59,'\xBF\xAA',6,0);"); //DS24(51,59,'开',6,0);
      Serial.print("SBC(56);DS24(51,129,'\xB9\xD8',0,0);"); //DS24(51,129,'关',0,0);
    } else {
      Serial.print("SBC(52);DS24(51,59,'\xBF\xAA',15,0);"); //DS24(51,59,'开',15,0);
      Serial.print("SBC(56);DS24(51,129,'\xB9\xD8',2,0);"); //DS24(51,129,'关',2,0);
    }
    if (autoC) {
      Serial.print("SBC(58);DS16(126,63,'\xD7\xD4\xB6\xAF\xC4\xA3\xCA\xBD',4,0);"); //DS16(126,63,'自动模式',4,0);
    } else {
      Serial.print("SBC(58);DS16(126,63,'\xD7\xD4\xB6\xAF\xC4\xA3\xCA\xBD',0,0);"); //DS16(126,63,'自动模式',0,0);
    }
    if (noAuto) {
      Serial.println("RTC\xC4\xA3\xBF\xE9\xB9\xCA\xD5\xCF"); //RTC模块故障
    } else {
      DateTime now = rtc.now();
      Serial.print("SBC(53);DS12(133,152,'"); Serial.print(now.hour()); Serial.print(":");
      if(now.minute() < 10) { Serial.print(0); Serial.print(now.minute()); } else Serial.print(now.minute()); Serial.print(":");
      if(now.second() < 10) { Serial.print(0); Serial.print(now.second()); } else Serial.print(now.second()); Serial.println("',16,0);"); //STIM(1,2,3);
    }
  }
  
  if (atCard) {
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) { mainLoop++; return; }
    if (!mfrc522.PICC_ReadCardSerial()) { mainLoop++; return; }
    // Select one of the cards
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    //printV(mfrc522.PICC_GetTypeName(piccType));
    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
       mainLoop++; return;
    }
    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[4][18];
    byte size = sizeof(buffer[0]);
    // Authenticate using key A
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        mainLoop++; return;
    }
    // Read data from the block
    for (blockAddr = 4; blockAddr < 8; blockAddr++) {
      status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer[blockAddr - 4], &size);
      if (status != MFRC522::STATUS_OK) { }
    }

    byte flag = 0;
    if (buffer[0][0] * 10 + buffer[0][1] < buffer[1][0] * 10 + buffer[1][1]) { // Zone B is bigger, use it as the first to read.
      flag = 1;
    }
    float current = 0.01; float last = 0.02;
    current = getBnc(buffer[flag]); 
    last = getBnc(buffer[1-flag]);
    Serial.print("CBOF(13,54,113,154,5,3);");
    Serial.print("SBC(3);");
    Serial.print("DS24(17,60,'\xD3\xE0\xB6\xEE',4,0);"); //DS24(17,60,'余额',4,0);
    Serial.print("DS24(58,82,'"); Serial.print(current); Serial.print("',4,0);");
    Serial.print("DS16(16,107,'\xC9\xCF\xB4\xCE\xCF\xFB\xB7\xD1',4,0);");
    Serial.print("DS24(58,124,'"); Serial.print(last - current); Serial.print("',4,0);");
    Serial.print("\r\n");

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
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
      case 12: if (tc >= 0503 && tc <= 0600 || tc >= 5500) off(); else on(); break;
      case 13: off(); break;
      case 14: if (tc >= 0000 && tc <= 2000) off(); else on(); break;
      case 15: if (tc >= 0503 && tc <= 0530) off(); else on(); break;
      case 16: if (tc >= 0003 && tc <= 0100 || tc >= 5003 && tc <= 5100) off(); else on(); break;
      case 17: if (tc >= 1000 && tc <= 2000) off(); else on(); break;
      case 18: case 19: case 20: case 21: case 22: case 23: on(); break;
    }
  }

}
mainLoop++;
}
