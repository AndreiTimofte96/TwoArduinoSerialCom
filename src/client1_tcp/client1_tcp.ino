// 32,16,8,0,Ana are mere si
#include "TcpProtocol.hpp"

TcpProtocol tcpProtocol;

void setup() {
  tcpProtocol.initializePorts(2, 3);                // RX, TX
  tcpProtocol.initializeSerial(Serial, 9600, 500);  //Serial, baudRate, Serial.setTimeout

  delay(1000);
}
char dataToReceive[300];
String str;
int strLength;

void loop() {
  if (!tcpProtocol.connect()) {
    tcpProtocol.printLastError();
  }

  while (1) {
    while (!Serial.available())
      ;
    str = Serial.readStringUntil('\n');
    strLength = str.length();
    char userStr[strLength + 1];
    str.toCharArray(userStr, strLength + 1);

    Serial.println(userStr);

    if (!tcpProtocol.write(userStr)) {
      tcpProtocol.printLastError();
    }

    if (!tcpProtocol.read(dataToReceive)) {
      tcpProtocol.printLastError();
    }

    Serial.println("\nRECEIVED DATA FROM SERVER:");
    Serial.println(dataToReceive);
    Serial.println();
  }

  tcpProtocol.arduinoClose();
}