#include "udpProtocol.h"

char CONNECTED_TEXT[] = "CONNECTED";
char DISCONNECTED_TEXT[] = "EROARE_LA_CONECTARE";
char READ_ERROR_TEXT[] = "EROARE_LA_CITIRE";
char WRITE_ERROR_TEXT[] = "EROARE_LA_SCRIERE";

UdpProtocol::UdpProtocol() {
  connection.setStatus(Connection::DISCONNECTED);
  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
}

///////////////
void UdpProtocol::waitRead() {
  while (!Serial.available())
    ;
}

void UdpProtocol::serialFlush() {
  byte b;
  while (Serial.readBytes(&b, 1) > 0)
    ;
}
///////////////

//////////////////
void UdpProtocol::setArduinoError(char *error) {
  ARDUINO_ERROR = (char *)malloc(sizeof(char) * strlen(error) + 1);
  memset(ARDUINO_ERROR, '\0', sizeof(char) * strlen(error) + 1);
  strcpy(ARDUINO_ERROR, error);
}

char *UdpProtocol::getArduinoError() {
  return ARDUINO_ERROR;
}

void UdpProtocol::printLastError() {
  Serial.print("EROARE: ");
  Serial.println(getArduinoError());
}
////////////////////

////////////////////
bool UdpProtocol::arduinoAcceptConnection() {
  if (connection.getStatus() == Connection::DISCONNECTED) {
    Serial.println();
    Serial.println("WAITING FOR CONNECTION");

    int connectedLength = strlen(CONNECTED_TEXT);
    char connected[connectedLength];

    waitRead();
    Serial.readBytes(connected, connectedLength);

    if (strcmp(connected, CONNECTED_TEXT) == 0) {
      Serial.write(CONNECTED_TEXT, connectedLength);
      connection.setStatus(Connection::CONNECTED);
      return true;
    }

    Serial.write(DISCONNECTED_TEXT, strlen(DISCONNECTED_TEXT));
    connection.setStatus(Connection::DISCONNECTED);
    setArduinoError(DISCONNECTED_TEXT);
    return false;
  }
  connection.setStatus(Connection::DISCONNECTED);
  setArduinoError(DISCONNECTED_TEXT);
  return false;
}

bool UdpProtocol::arduinoConnect() {
  if (connection.getStatus() == Connection::DISCONNECTED) {
    Serial.write(CONNECTED_TEXT, strlen(CONNECTED_TEXT));
    waitRead();

    int connectedLength = strlen(CONNECTED_TEXT);
    char connected[connectedLength];
    Serial.readBytes(connected, connectedLength);
    Serial.println(connected);

    if (strcmp(connected, CONNECTED_TEXT) == 0) {
      connection.setStatus(Connection::CONNECTED);

      return true;
    };
  }

  connection.setStatus(Connection::DISCONNECTED);

  setArduinoError(DISCONNECTED_TEXT);
  return false;
}

bool UdpProtocol::arduinoServerClose() {
  serialFlush();
  connection.setStatus(Connection::IDLE);
}

bool UdpProtocol::arduinoClose() {
  while (1)
    ;
  connection.setStatus(Connection::IDLE);
}
///////////////////////

////////////////
void UdpProtocol::addNumberToCharArray(int number, char *str) {
  char aux[100];
  sprintf(aux, "%d,", number);
  strcat(str, aux);
}

void UdpProtocol::addOffsetToCharArray(char *str) {
  int strLength = strlen(str);
  for (int index = strLength; index < UdpPacket::BLOCK_HEADER_SIZE; index++) {
    str[index] = ',';
  }
}

void UdpProtocol::formatSendData(char *packet) {
  addNumberToCharArray(packetWrite.pSize, packet);
  addNumberToCharArray(packetWrite.bLength, packet);
  addNumberToCharArray(packetWrite.bNumber, packet);
  addNumberToCharArray(packetWrite.bOffset, packet);
  addOffsetToCharArray(packet);
  strcat(packet, packetWrite.bData);
}

void UdpProtocol::sendData(char *dataToSend) {
  packetWrite.bNumber = (strlen(dataToSend) / packetWrite.bLength) + 1;
  int remainder = strlen(dataToSend) % packetWrite.bLength;
  int bLength = packetWrite.bLength;
  int blockLength = bLength;
  for (int blockIndex = 0; blockIndex < packetWrite.bNumber; blockIndex++) {
    if (blockIndex == packetWrite.bNumber - 1) {
      packetWrite.bLength = remainder;
      blockLength = remainder;
    }
    packetWrite.bOffset = blockIndex;
    packetWrite.bData = (char *)malloc(sizeof(char) * packetWrite.bLength + 1);
    memset(packetWrite.bData, '\0', sizeof(char) * packetWrite.bLength + 1);

    for (int dataIndex = blockIndex * bLength; dataIndex < blockIndex * bLength + blockLength; dataIndex++) {
      int bDataLength = strlen(packetWrite.bData);
      packetWrite.bData[bDataLength] = dataToSend[dataIndex];
    }
    char packet[packetWrite.pSize];
    memset(packet, '\0', sizeof(char) * packetWrite.pSize);
    formatSendData(packet);
    Serial.write(packet, strlen(packet));
  }
}

bool UdpProtocol::udpWrite(char *dataToSend) {
  if (connection.getStatus() == Connection::CONNECTED) {
    sendData(dataToSend);
    return true;
  }
  setArduinoError(WRITE_ERROR_TEXT);
  return false;
}
////////////////////

/////////////////////////
void UdpProtocol::formatReceiveData(char *bData, char *dataToReceive) {
  char *pch;
  pch = strtok(bData, ",");
  packetRead.pSize = atoi(pch);

  pch = strtok(NULL, ",");
  packetRead.bLength = atoi(pch);

  pch = strtok(NULL, ",");
  packetRead.bNumber = atoi(pch);

  pch = strtok(NULL, ",");
  packetRead.bOffset = atoi(pch);

  pch = strtok(NULL, ",");
  packetRead.bData = pch;

  packetRead.bData[packetRead.bLength] = '\0';
  strcat(dataToReceive, packetRead.bData);
}

void UdpProtocol::receiveData(char *dataToReceive) {
  int bDataLength = UdpPacket::BLOCK_SIZE;
  char bData[bDataLength];

  // serialFlush();
  waitRead();
  memset(bData, '\0', sizeof(char) * bDataLength);
  Serial.readBytes(bData, bDataLength);
  formatReceiveData(bData, dataToReceive);

  while (packetRead.bOffset + 1 < packetRead.bNumber) {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    Serial.readBytes(bData, bDataLength);
    formatReceiveData(bData, dataToReceive);
  }
}

bool UdpProtocol::udpRead(char *dataToReceive) {
  if (connection.getStatus() == Connection::CONNECTED) {
    memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
    receiveData(dataToReceive);
    return true;
  }
  setArduinoError(READ_ERROR_TEXT);
  return false;
}
//////////////