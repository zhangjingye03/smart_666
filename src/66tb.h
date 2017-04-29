#include <Arduino.h>
#include <Wire.h>
#include <AT24CX.h>
#define random(x) (rand()%x)
AT24CX mem;
int count, verYear, verDate;

char tellMeAns (char ans) {
  switch (ans) {
    case 0: return 'A';
    case 1: return 'B';
    case 2: return 'C';
    case 3: return 'D';
  }
}

char tellMeSubj (char subj) {
  switch (subj) {
    case 0: Serial.print("\xCE\xB4\xD6\xAA"); break; //未知
    case 1: Serial.print("\xD3\xEF\xCE\xC4"); break; //语文
    case 2: Serial.print("\xCA\xFD\xD1\xA7"); break; //数学
    case 3: Serial.print("\xD3\xA2\xD3\xEF"); break; //英语
    case 4: Serial.print("\xCE\xEF\xC0\xED"); break; //物理
    case 5: Serial.print("\xBB\xAF\xD1\xA7"); break; //化学
    case 6: Serial.print("\xC9\xFA\xCE\xEF"); //生物
  }
}

void readInBlock (int addr, char len/*, char** data*/) {
  char context[len + 1]; // +1 for \0
  mem.readChars(addr, context, len);
  context[len] = 0x0;
  Serial.print(context);
}

char readRelatively (int naddr) {
  char subject = mem.read(naddr);
  char sizeQ = mem.read(naddr + 1);
  char sizeA = mem.read(naddr + 2);
  char sizeB = mem.read(naddr + 3);
  char sizeC = mem.read(naddr + 4);
  char sizeD = mem.read(naddr + 5);
  char right = mem.read(naddr + 6);
  if (sizeQ > 100) { // Unable to handle such many texts in one BS12.
    Serial.print(F("BS12(8,28,172,4,'[")); tellMeSubj(subject); Serial.print("] ");
    readInBlock(naddr + 7, 100);
    Serial.print(F("');BS12(8,91,172,4,'"));
    readInBlock(naddr + 7, sizeQ - 100);
  } else {
    Serial.print(F("BS16(8,40,172,4,'[")); tellMeSubj(subject); Serial.print("] ");
    readInBlock(naddr + 7, sizeQ);
  }
  Serial.print(F("',16);SBC(53);BS12(4,160,41,4,'"));
  readInBlock( naddr + 7 + sizeQ, sizeA ); // A
  Serial.print(F("',16);SBC(22);BS12(48,160,85,4,'"));
  readInBlock( naddr + 7 + sizeQ + sizeA, sizeB ); // B
  Serial.print(F("',16);SBC(55);BS12(92,160,129,4,'"));
  readInBlock( naddr + 7 + sizeQ + sizeA + sizeB, sizeC ); // C
  Serial.print(F("',16);SBC(56);BS12(136,160,173,4,'"));
  readInBlock( naddr + 7 + sizeQ + sizeA + sizeB + sizeC, sizeD ); // D
  Serial.print(F("',16);SBC(11);DS12(0,1,'\xCC\xE2\xBF\xE2\xB0\xE6\xB1\xBE',15);DS12(0,11,'"));
  Serial.print(verYear, DEC); Serial.write (' '); Serial.print(verDate, DEC);
  Serial.println(F("',15);"));
  return right;
}


char readQuestion(int qc) {
  int qAddr[count];
  for (int i = 0; i < count; i++) {
    qAddr[i] = mem.readInt( 0x0006 + i * 2 );
  }
  Serial.println(F("SPG(15);")); delay(50);
  Serial.print(F("SBC(11);LABL(24,30,0,145,'"));
  Serial.print(qc + 1, DEC); Serial.print("/"); Serial.print(count, DEC);
  Serial.print(F("',15,1);"));
  return readRelatively(qAddr[qc]);
}
