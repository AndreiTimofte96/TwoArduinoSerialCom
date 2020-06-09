// UAID = 8808
#include "TcpProtocol.hpp"

TcpProtocol tcpProtocol;

int thisUAID = TcpProtocol::getUniqueArduinoIDFromEEEPROM();

char dataToSend[] = "Salut, eu sunt Andrei si sunt student la Facultatea de Informatica Iasi!";
char dataToReceive[300];
int destinationUAID = 0;
int length;

String str;
int strLength;

void setup() {
  tcpProtocol.initializePorts(2, 3);                       // RX, TX
  tcpProtocol.initializeSerial(Serial, 9600, 500, false);  //Serial, baudRate, Serial.setTimeout
}

float start = 0, end = 0;

void loop() {
  if (tcpProtocol.connect() < 0) {
    tcpProtocol.printLastError();
  }

  while (1) {
    start = millis();
    if ((length = tcpProtocol.write(dataToSend, thisUAID)) < 0) {
      tcpProtocol.printLastError();
    }

    if ((length = tcpProtocol.read(dataToReceive, destinationUAID)) < 0) {
      tcpProtocol.printLastError();
    }
    end = millis();

    Serial.println("DONE");
    Serial.println(dataToReceive);
    Serial.println(end - start);
  }

  if (tcpProtocol.clientClose() < 0) {
    tcpProtocol.printLastError();
  }  // else

  Serial.println(tcpProtocol.getConnectionStatus());
  while (1)
    ;
}

// Salut, eu sunt Andrei si sunt student la Facultatea de Informatica Iasi!
// Salut, eu sunt Andrei si sunt student la Facultatea de Informatica Iasi si in luna iulie voi prezenta lucrara de licenta
//Salut, eu sunt Andrei si sunt student la Facultatea de Informatica!