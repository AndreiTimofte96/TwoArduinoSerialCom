// UAID = 8808
#include "TcpProtocol.hpp"

TcpProtocol tcpProtocol;
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

  tcpProtocol.initializePorts(receiver[0].rxPort, receiver[0].txPort);  // RX, TX
  tcpProtocol.initializeSerial(Serial, 9600, 500);                      //Serial, baudRate, Serial.setTimeout
}

char dataToReceive[300];
int destinationUAID = 0;
String str;
int strLength;

void loop() {
  if (!tcpProtocol.connect()) {
    tcpProtocol.printLastError();
  }

  int ok = 2;
  while (ok) {
    ok--;
    while (!Serial.available())
      ;
    str = Serial.readStringUntil('\n');
    strLength = str.length();
    char userInput[strLength + 1];
    str.toCharArray(userInput, strLength + 1);

    Serial.println(userInput);

    if (!tcpProtocol.write(userInput, receiver[0].UAID)) {
      tcpProtocol.printLastError();
    }

    if (!tcpProtocol.read(dataToReceive, destinationUAID)) {
      tcpProtocol.printLastError();
    }

    Serial.print("\nRECEIVED DATA THROUGH SERVER FROM CLIENT ");
    Serial.print(destinationUAID);
    Serial.println(":");
    Serial.println(dataToReceive);
    Serial.println();
  }

  tcpProtocol.clientClose();
  while (1)
    ;
}

// Salut, eu sunt Andrei si sunt student la Facultatea de Informatica Iasi!