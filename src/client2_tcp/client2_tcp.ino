// UAID = 4987
#include <TcpProtocol.hpp>

TcpProtocol tcpProtocol;

int thisUAID = TcpProtocol::getUniqueArduinoIDFromEEEPROM();

char dataToReceive[300];
int destinationUAID = 0;  //should be 8808;
int length;

String str;
int strLength;

// long baudRate = 115200;
// long baudRate = 74800;
// long baudRate = 57600;
// long baudRate = 38400;
// long baudRate = 19200;
long baudRate = 9600;

void setup() {
  tcpProtocol.initializePorts(2, 3);                    // RX, TX
  tcpProtocol.initializeSerial(Serial, baudRate, 500);  //Serial, baudRate, Serial.setTimeout
  // tcpProtocol.useShowLogs(true);
}

float start = 0, end = 0;

void loop() {
  if ((destinationUAID = tcpProtocol.listen()) < 0) {
    tcpProtocol.printLastError();
  }

  while (1) {
    if ((length = tcpProtocol.read(dataToReceive, destinationUAID)) < 0) {
      tcpProtocol.printLastError();
    }

    if ((length = tcpProtocol.write(dataToReceive, thisUAID)) < 0) {
      tcpProtocol.printLastError();
    }
  }

  if (tcpProtocol.serverClose() < 0) {
    tcpProtocol.printLastError();
  }

  while (1)
    ;
}