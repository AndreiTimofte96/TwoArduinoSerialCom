// UAID = 4987
#include "UdpProtocol.hpp"

UdpProtocol udpProtocol;

struct {
  int rxPort;
  int txPort;
  int UAID;
} receiver[2];

void receiversSetup() {
  receiver[0].rxPort = 8;
  receiver[0].txPort = 9;
  receiver[0].UAID = 8808;
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
  int ok = 2;
  int length;
  while (ok) {
    ok--;
    if ((length = udpProtocol.read(dataToReceive, fromUAID)) < 0) {
      udpProtocol.printLastError();
    }

    Serial.print("\nRECEIVED DATA THROUGH SERVER FROM CLIENT ");
    Serial.print(fromUAID);
    Serial.println(":");
    Serial.println(dataToReceive);
    Serial.println(length);
    Serial.println();

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
  }

  while (1)
    ;
}
