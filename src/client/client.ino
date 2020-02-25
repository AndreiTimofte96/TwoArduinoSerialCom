// 32,16,8,0,Ana are mere si
#include "UdpProtocol.hpp"

UdpProtocol udpProtocol;

void setup() {
  udpProtocol.initializePorts(2, 3);                // RX, TX
  udpProtocol.initializeSerial(Serial, 9600, 500);  //Serial, baudRate, Serial.setTimeout

  delay(2000);
}

char dataToReceive[1000];
char option[] = "2";
void loop() {
  if (!udpProtocol.udpWrite(option)) {
    udpProtocol.printLastError();
  }

  if (!udpProtocol.udpRead(dataToReceive)) {
    udpProtocol.printLastError();
  }
  Serial.println("\nRECEIVED DATA:");
  Serial.println(dataToReceive);
  Serial.println("END");

  udpProtocol.arduinoClose();
}
