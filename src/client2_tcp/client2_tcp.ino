// UAID = 4987
#include "TcpProtocol.hpp"

TcpProtocol tcpProtocol;

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

  tcpProtocol.initializePorts(receiver[0].rxPort, receiver[0].txPort);  // RX, TX
  tcpProtocol.initializeSerial(Serial, 9600, 500, false);               //Serial, baudRate, Serial.setTimeout
}

char dataToReceive[300];
int destinationUAID = 0;
String str;
int strLength;

void loop() {
  if (tcpProtocol.connect() < 0) {
    tcpProtocol.printLastError();
  }

  int ok = 2;
  int length;
  while (ok) {
    ok--;
    if ((length = tcpProtocol.read(dataToReceive, destinationUAID)) < 0) {
      tcpProtocol.printLastError();
    }

    Serial.print("\nRECEIVED DATA THROUGH SERVER FROM CLIENT ");
    Serial.print(destinationUAID);
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

    if ((length = tcpProtocol.write(userInput, receiver[0].UAID)) < 0) {
      tcpProtocol.printLastError();
    }
    Serial.println("WRITE LENGTH");
    Serial.println(length);
  }

  if (tcpProtocol.clientClose() < 0) {
    tcpProtocol.printLastError();
  }  // else
  Serial.println(tcpProtocol.getConnectionStatus());
  while (1)
    ;
}

//Salut, ma bucur sa te cunosc!