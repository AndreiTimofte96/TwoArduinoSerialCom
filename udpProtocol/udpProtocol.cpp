#include "udpProtocol.h"

UdpProtocol::UdpProtocol() {
  // connection.setStatus(Connection::DISCONNECTED);
  connection.setStatus(Connection::CONNECTED);

  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
<<<<<<< HEAD
  // strcpy(specialChr, ",");
  strcpy(specialChr, "\f");
=======
>>>>>>> add Serial port dynamic selection
}

void UdpProtocol::initPort(HardwareSerial &Serial, int beginSpeed, int timeout = 1000) {
  _Serial = &Serial;
  _Serial->begin(beginSpeed);
  _Serial->setTimeout(timeout);
}

char CONNECTED_TEXT[] = "CONNECTED";

///////////////
void UdpProtocol::waitRead() {
<<<<<<< HEAD
  while (!Serial.available())
=======
  while (!_Serial->available())
>>>>>>> add Serial port dynamic selection
    ;
}

void UdpProtocol::serialFlush() {
  byte b;
<<<<<<< HEAD
  while (Serial.readBytes(&b, 1) > 0)
=======
  while (_Serial->readBytes(&b, 1) > 0)
>>>>>>> add Serial port dynamic selection
    ;
}

void UdpProtocol::println(char *data) {
<<<<<<< HEAD
  Serial.println(data);
}

void UdpProtocol::println(int data) {
  Serial.println(data);
=======
  _Serial->println(data);
}

void UdpProtocol::println(int data) {
  _Serial->println(data);
>>>>>>> add Serial port dynamic selection
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
<<<<<<< HEAD
  Serial.print("EROARE: ");
  Serial.println(error.getError());
=======
  _Serial->print("EROARE: ");
  _Serial->println(error.getError());
>>>>>>> add Serial port dynamic selection
}
////////////////////

////////////////////
bool UdpProtocol::arduinoAcceptConnection() {
  if (connection.getStatus() == Connection::DISCONNECTED) {
<<<<<<< HEAD
    Serial.println();
    Serial.println("WAITING FOR CONNECTION");
=======
    _Serial->println();
    _Serial->println("WAITING FOR CONNECTION");
>>>>>>> add Serial port dynamic selection

    int connectedLength = strlen(CONNECTED_TEXT);
    char connected[connectedLength];

    waitRead();
<<<<<<< HEAD
    Serial.readBytes(connected, connectedLength);

    if (strcmp(connected, CONNECTED_TEXT) == 0) {
      Serial.write(CONNECTED_TEXT, connectedLength);
=======
    _Serial->readBytes(connected, connectedLength);

    if (strcmp(connected, CONNECTED_TEXT) == 0) {
      _Serial->write(CONNECTED_TEXT, connectedLength);
>>>>>>> add Serial port dynamic selection
      connection.setStatus(Connection::CONNECTED);
      return true;
    }

    connection.setStatus(Connection::DISCONNECTED);
    error.setError(Error::CONNECTION_ERROR);
<<<<<<< HEAD
    Serial.write(error.getError(), strlen(error.getError()));
=======
    _Serial->write(error.getError(), strlen(error.getError()));
>>>>>>> add Serial port dynamic selection

    return false;
  }
  connection.setStatus(Connection::DISCONNECTED);
  error.setError(Error::CONNECTION_ERROR);

  return false;
}

bool UdpProtocol::arduinoConnect() {
  if (connection.getStatus() == Connection::DISCONNECTED) {
<<<<<<< HEAD
    Serial.write(CONNECTED_TEXT, strlen(CONNECTED_TEXT));
=======
    _Serial->write(CONNECTED_TEXT, strlen(CONNECTED_TEXT));
>>>>>>> add Serial port dynamic selection
    waitRead();

    int connectedLength = strlen(CONNECTED_TEXT);
    char connected[connectedLength];
<<<<<<< HEAD
    Serial.readBytes(connected, connectedLength);
    Serial.println(connected);
=======
    _Serial->readBytes(connected, connectedLength);
    _Serial->println(connected);
>>>>>>> add Serial port dynamic selection

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
<<<<<<< HEAD
  sprintf(aux, "%d", number);
  strcat(str, aux);
  strcat(str, specialChr);
=======
  sprintf(aux, "%d,", number);
  strcat(str, aux);
>>>>>>> add Serial port dynamic selection
}

void UdpProtocol::addOffsetToCharArray(char *str) {
  int strLength = strlen(str);
  for (int index = strLength; index < UdpPacket::BLOCK_HEADER_SIZE; index++) {
<<<<<<< HEAD
    str[index] = specialChr[0];
=======
    str[index] = ',';
>>>>>>> add Serial port dynamic selection
  }
}

void UdpProtocol::formatSendData(char *packet) {
<<<<<<< HEAD
  int checkSum1 = packetWrite.checkSum1;
  int checkSum2 = packetWrite.checkSum2;

  computeChecksum(packetWrite.bData, checkSum1, checkSum2);

=======
>>>>>>> add Serial port dynamic selection
  addNumberToCharArray(packetWrite.pSize, packet);
  addNumberToCharArray(packetWrite.bLength, packet);
  addNumberToCharArray(packetWrite.bNumber, packet);
  addNumberToCharArray(packetWrite.bOffset, packet);
<<<<<<< HEAD
  addNumberToCharArray(checkSum1, packet);
  addNumberToCharArray(checkSum2, packet);
=======
>>>>>>> add Serial port dynamic selection
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
<<<<<<< HEAD

    formatSendData(packet);
    Serial.write(packet, strlen(packet));
=======
    formatSendData(packet);
    _Serial->write(packet, strlen(packet));
>>>>>>> add Serial port dynamic selection
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
<<<<<<< HEAD
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
=======
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
>>>>>>> add Serial port dynamic selection
}

void UdpProtocol::receiveData(char *dataToReceive) {
  int bDataLength = UdpPacket::BLOCK_SIZE;
  char bData[bDataLength];
<<<<<<< HEAD

  // serialFlush();
  waitRead();
  memset(bData, '\0', sizeof(char) * bDataLength);
  Serial.readBytes(bData, bDataLength);
  formatReceiveData(bData, dataToReceive);

=======

  // serialFlush();
  waitRead();
  memset(bData, '\0', sizeof(char) * bDataLength);
  _Serial->readBytes(bData, bDataLength);
  formatReceiveData(bData, dataToReceive);

>>>>>>> add Serial port dynamic selection
  while (packetRead.bOffset + 1 < packetRead.bNumber) {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    _Serial->readBytes(bData, bDataLength);
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