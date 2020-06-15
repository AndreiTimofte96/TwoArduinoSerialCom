// UAID = 3607
#include <UdpProtocol.hpp>

UdpProtocol udpProtocol1, udpProtocol2;

struct {
  int rxPort;
  int txPort;
  int UAID;
} clients[2];

char dataToReceive[300];
int fromUAID, toUAID;
int length;

void clientsSetup() {
  clients[0].rxPort = 2;
  clients[0].txPort = 3;

  clients[1].rxPort = 8;
  clients[1].txPort = 9;
}

void setup() {
  clientsSetup();

  udpProtocol1.initializePorts(clients[0].rxPort, clients[0].txPort);  // RX, TX
  udpProtocol1.initializeSerial(Serial, 9600, 500);                    //Serial, baudRate, Serial.setTimeout
  udpProtocol1.useShowLogs(true);
  udpProtocol1.useAsyncMode(true);

  udpProtocol2.initializePorts(clients[1].rxPort, clients[1].txPort);  // RX, TX
  udpProtocol2.initializeSerial(Serial, 9600, 500);
  udpProtocol2.useShowLogs(true);
  udpProtocol2.useAsyncMode(true);
}

void loop() {
  int ok = 2;
  int length;

  length = udpProtocol1.read(dataToReceive, fromUAID, toUAID);
  if (length < 0) {
    udpProtocol1.printLastError();
  }
  if (length > 0) {
    Serial.println("\nRECEIVED DATA FROM CLIENT 1:");
    Serial.println(dataToReceive);
    Serial.println(fromUAID);

    if ((length = udpProtocol2.write(dataToReceive, fromUAID, toUAID)) < 0) {
      udpProtocol2.printLastError();
    }
  }

  length = udpProtocol2.read(dataToReceive, fromUAID, toUAID);
  if (length < 0) {
    udpProtocol2.printLastError();
  }
  if (length > 0) {
    Serial.println("\nRECEIVED DATA FROM CLIENT 2:");
    Serial.println(fromUAID);

    if ((length = udpProtocol1.write(dataToReceive, fromUAID, toUAID)) < 0) {
      udpProtocol1.printLastError();
    }
  }
}