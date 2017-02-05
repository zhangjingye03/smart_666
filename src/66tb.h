#include <Arduino.h>
#include <Wire.h>
#include <AT24CX.h>
#define random(x) (rand()%x)
AT24C32 mem;
int count;

char tellMeAns(byte ans) {
  switch (ans) {
    case 0: return 'A';
    case 1: return 'B';
    case 2: return 'C';
    case 3: return 'D';
  }
}

void readInBlock (int addr, byte len/*, char** data*/) {
  char context[len + 1]; // +1 for \0
  mem.readChars(addr, context, len);
  context[len] = 0x0;
  Serial.print(context);
}

char readRelatively (int naddr) {
  byte sizeQ = mem.read(naddr);
  byte sizeA = mem.read(naddr + 1);
  byte sizeB = mem.read(naddr + 2);
  byte sizeC = mem.read(naddr + 3);
  byte sizeD = mem.read(naddr + 4);
  byte right = mem.read(naddr + 5);
  if (sizeQ > 100) { // Unable to handle such many texts in one BS12.
    Serial.print(F("BS12(4,28,172,4,'"));
    readInBlock(naddr + 6, 100);
    Serial.print(F("');BS12(4,91,172,4,'"));
    readInBlock(naddr + 6, sizeQ - 100);
  } else {
    Serial.print(F("BS12(4,60,172,4,'"));
    readInBlock(naddr + 6, sizeQ);
  }
  Serial.print(F("',16);SBC(53);BS12(4,160,41,4,'"));
  readInBlock( naddr + 6 + sizeQ, sizeA ); // A
  Serial.print(F("',16);SBC(22);BS12(48,160,85,4,'"));
  readInBlock( naddr + 6 + sizeQ + sizeA, sizeB ); // B
  Serial.print(F("',16);SBC(55);BS12(92,160,129,4,'"));
  readInBlock( naddr + 6 + sizeQ + sizeA + sizeB, sizeC ); // C
  Serial.print(F("',16);SBC(56);BS12(136,160,173,4,'"));
  readInBlock( naddr + 6 + sizeQ + sizeA + sizeB + sizeC, sizeD ); // D
  Serial.println(F("',16);"));

  return right;
}


char readQuestion(int qc) {
  int qAddr[count];
  for (int i = 0; i < count; i++) {
    qAddr[i] = mem.readInt( 0x0002 + i * 2 );
  }
  Serial.println(F("SPG(15);")); delay(50);
  Serial.print(F("SBC(11);LABL(24,30,0,145,'"));
  Serial.print(qc + 1, DEC); Serial.print("/"); Serial.print(count, DEC);
  Serial.print(F("',15,1);"));
  return readRelatively(qAddr[qc]);
}
