#include "udpProtocol.h"

UdpProtocol::UdpProtocol() {
  // connection.setStatus(Connection::DISCONNECTED);
  connection.setStatus(Connection::CONNECTED);

  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
  // strcpy(specialChr, ",");
  strcpy(specialChr, "\f");
}
char CONNECTED_TEXT[] = "CONNECTED";

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

void UdpProtocol::println(char *data) {
  Serial.println(data);
}

void UdpProtocol::println(int data) {
  Serial.println(data);
}

void UdpProtocol::computeChecksum(char *data, int &checkSum1, int &checkSum2) {
  for (int index = 0; index < strlen(data); index++) {
    checkSum1 = (checkSum1 + data[index]) % 255;
    checkSum2 += checkSum1 % 255;
  }
  checkSum1 %= 255;
  checkSum2 %= 255;
}

bool UdpProtocol::hasPacketErrors(char *data) {
  int checkSum1 = 0, checkSum2 = 0;
  computeChecksum(data, checkSum1, checkSum2);
  if (checkSum1 == packetRead.checkSum1 && checkSum2 == packetRead.checkSum2) {
    return false;
  }
  return true;
}

///////////////

//////////////////

void UdpProtocol::printLastError() {
  Serial.print("EROARE: ");
  Serial.println(error.getError());
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

    connection.setStatus(Connection::DISCONNECTED);
    error.setError(Error::CONNECTION_ERROR);
    Serial.write(error.getError(), strlen(error.getError()));

    return false;
  }
  connection.setStatus(Connection::DISCONNECTED);
  error.setError(Error::CONNECTION_ERROR);

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

  error.setError(Error::CONNECTION_ERROR);
  return false;
}

bool UdpProtocol::arduinoServerClose() {
  while (1)
    ;
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
  sprintf(aux, "%d", number);
  strcat(str, aux);
  strcat(str, specialChr);
}

void UdpProtocol::addOffsetToCharArray(char *str) {
  int strLength = strlen(str);
  for (int index = strLength; index < UdpPacket::BLOCK_HEADER_SIZE; index++) {
    str[index] = specialChr[0];
  }
}

void UdpProtocol::formatSendData(char *packet) {
  int checkSum1 = packetWrite.checkSum1;
  int checkSum2 = packetWrite.checkSum2;

  computeChecksum(packetWrite.bData, checkSum1, checkSum2);

  addNumberToCharArray(packetWrite.pSize, packet);
  addNumberToCharArray(packetWrite.bLength, packet);
  addNumberToCharArray(packetWrite.bNumber, packet);
  addNumberToCharArray(packetWrite.bOffset, packet);
  addNumberToCharArray(checkSum1, packet);
  addNumberToCharArray(checkSum2, packet);
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
  error.setError(Error::WRITE_ERROR);
  return false;
}
////////////////////

/////////////////////////
void UdpProtocol::formatReceiveData(char *bData, char *dataToReceive) {
  char *pch;
  pch = strtok(bData, specialChr);
  packetRead.pSize = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bLength = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bNumber = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bOffset = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.checkSum1 = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.checkSum2 = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bData = pch;

  packetRead.bData[packetRead.bLength] = '\0';

  if (!hasPacketErrors(packetRead.bData)) {
    strcat(dataToReceive, packetRead.bData);
  } else {
    char err[100];
    snprintf(err, 100, "ERR_PACKET_%d_%d_[%s]", packetRead.checkSum1, packetRead.checkSum2, packetRead.bData);
    strcat(dataToReceive, err);
  }
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
  error.setError(Error::READ_ERROR);

  return false;
}
//////////////