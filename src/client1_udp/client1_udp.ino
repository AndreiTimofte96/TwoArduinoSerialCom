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
  udpProtocol.useShowLogs(false);
  udpProtocol.useAsyncMode(true);
}

char dataToReceive[300];
int fromUAID = 0;
String str;
int length, strLength;

void loop() {
  if (Serial.available()) {
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
  }

  length = udpProtocol.read(dataToReceive, fromUAID);
  if (length < 0) {
    udpProtocol.printLastError();
  }
  if (length > 0) {
    Serial.print("\nRECEIVED DATA THROUGH SERVER FROM CLIENT ");
    Serial.print(fromUAID);
    Serial.println(":");
    Serial.println(dataToReceive);
    Serial.println(length);
    Serial.println();
  }
}