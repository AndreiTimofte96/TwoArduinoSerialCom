// UAID = 8808
#include "UdpProtocol.hpp"

UdpProtocol udpProtocol;

struct {
  int rxPort;
  int txPort;
  int UAID;
} receiver[2];

void receiversSetup() {
  receiver[0].rxPort = 2;
  receiver[0].txPort = 3;
  receiver[0].UAID = 4987;
}

void setup() {
  receiversSetup();
  udpProtocol.initializePorts(receiver[0].rxPort, receiver[0].txPort);  // RX, TX
  udpProtocol.initializeSerial(Serial, 9600, 500);                      //Serial, baudRate, Serial.setTimeout
}

char dataToReceive[300];
int fromUAID = 0;
String str;
int strLength;

void loop() {
  if (udpProtocol.connect() < 0) {
    udpProtocol.printLastError();
  }

  int ok = 2;
  int length;
  while (ok) {
    ok--;
    while (!Serial.available())
      ;
    str = Serial.readStringUntil('\n');
    strLength = str.length();
    char userInput[strLength + 1];
    str.toCharArray(userInput, strLength + 1);

    Serial.println(userInput);

    if ((length = udpProtocol.write(userInput, receiver[0].UAID)) < 0) {
      udpProtocol.printLastError();
    }
    Serial.println("WRITE LENGTH");
    Serial.println(length);

    if (!udpProtocol.read(dataToReceive, fromUAID)) {
      udpProtocol.printLastError();
    }

    Serial.print("\nRECEIVED DATA THROUGH SERVER FROM CLIENT ");
    Serial.print(fromUAID);
    Serial.println(":");
    Serial.println(dataToReceive);
    Serial.println(length);
    Serial.println();
  }

  if (udpProtocol.clientClose() < 0) {
    udpProtocol.printLastError();
  }
  while (1)
    ;
}
