// 32,16,8,0,Ana are mere si
#include "TcpProtocol.hpp"

TcpProtocol tcpProtocol;

void setup() {
  tcpProtocol.initializePorts(2, 3);                // RX, TX
  tcpProtocol.initializeSerial(Serial, 9600, 500);  //Serial, baudRate, Serial.setTimeout

  delay(1000);
}

char dataToReceive[300];
char option[] = "2";

void loop() {
  if (!tcpProtocol.connect()) {
    tcpProtocol.printLastError();
  }

  if (!tcpProtocol.write(option)) {
    tcpProtocol.printLastError();
  }

  if (!tcpProtocol.read(dataToReceive)) {
    tcpProtocol.printLastError();
  }
  Serial.println("\nRECEIVED DATA:");
  Serial.println(dataToReceive);
  Serial.println("END");

  tcpProtocol.arduinoClose();
}
