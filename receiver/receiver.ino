// 32,16,8,0,Ana are mere si
#include "udpProtocol.h"

UdpProtocol udpProtocol;

void setup() {
  udpProtocol.initPort(Serial, 9600, 500);
  delay(5000);
}

char dataToReceive[1000];
char option[] = "2";
void loop() {
  // if (!udpProtocol.arduinoConnect()) {
  //   udpProtocol.printLastError();
  // }

  if (!udpProtocol.udpWrite(option)) {
    udpProtocol.printLastError();
  }

  if (!udpProtocol.udpRead(dataToReceive)) {
    udpProtocol.printLastError();
  }
  Serial.println("RECEIVED DATA:");
  Serial.println(dataToReceive);
  Serial.println("END");
  udpProtocol.arduinoClose();
}
