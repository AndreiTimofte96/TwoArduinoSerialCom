#include "TcpProtocol.hpp"

TcpProtocol::TcpProtocol() {
  packetWrite.pSize = TcpPacket::BLOCK_SIZE;
  packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE;
}

void TcpProtocol::listen() {
  establishConnectionServer();
}

bool TcpProtocol::connect() {
  return establishConnectionClient();
}

void TcpProtocol::addNumberToCharArray(int number, char *str) {
  char aux[100];
  sprintf(aux, "%d", number);
  strcat(str, aux);
  strcat(str, specialChr);
}

void TcpProtocol::addOffsetToCharArray(char *str) {
  int strLength = strlen(str);
  for (int index = strLength; index < TcpPacket::BLOCK_HEADER_SIZE; index++) {
    str[index] = specialChr[0];
  }
}

void TcpProtocol::formatSendData(char *packet) {
  // int checkSum1 = packetWrite.checkSum1;
  // int checkSum2 = packetWrite.checkSum2;

  // computeChecksum(packetWrite.bData, checkSum1, checkSum2);

  addNumberToCharArray(packetWrite.pSize, packet);
  addNumberToCharArray(packetWrite.bLength, packet);
  addNumberToCharArray(packetWrite.bNumber, packet);
  addNumberToCharArray(packetWrite.bOffset, packet);
  addNumberToCharArray(packetWrite.pKind, packet);
  addNumberToCharArray(packetWrite.ack, packet);
  addNumberToCharArray(packetWrite.checkSum2, packet);
  addNumberToCharArray(packetWrite.checkSum2, packet);
  addOffsetToCharArray(packet);
  strcat(packet, packetWrite.bData);
}

void TcpProtocol::sendData(char *dataToSend) {
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

    softwareSerial->write(packet, strlen(packet));
    delay(100);
    hardwareSerial->println();
    hardwareSerial->println(packet);
  }
}

bool TcpProtocol::establishConnectionClient() {  //THREE WAY HANDSHAKE
  char packet[packetWrite.pSize];
  packetWrite.bNumber = 1;
  packetWrite.bOffset = 9001;
  packetWrite.pKind = 1;  // /ACK:0, SEQ:1 CON:2 FIN:3
  packetWrite.ack = 0;
  packetWrite.checkSum1 = 0;
  packetWrite.checkSum2 = 0;
  packetWrite.bData = (char *)malloc(sizeof(char) * packetWrite.bLength + 1);
  memset(packetWrite.bData, '\0', sizeof(char) * packetWrite.bLength + 1);

  memset(packet, '\0', sizeof(char) * packetWrite.pSize);
  formatSendData(packet);

  int bDataLength = TcpPacket::BLOCK_SIZE;
  char bData[bDataLength];

  softwareSerial->write(packet, strlen(packet));
  delay(100);
  hardwareSerial->println();
  hardwareSerial->println(packet);

  waitRead();
  memset(bData, '\0', sizeof(char) * bDataLength);
  softwareSerial_readBytes(bData, bDataLength);
  char *dataToReceive;
  formatReceiveData(bData, dataToReceive);

  if (packetRead.pKind == 1 && packetRead.ack == packetWrite.bOffset + 1) {
    packetWrite.bNumber = 1;
    packetWrite.bOffset = packetRead.bOffset;
    packetWrite.pKind = 0;  // /ACK:0, SEQ:1 CON:2 FIN:3
    packetWrite.ack = packetRead.bOffset + 1;
    packetWrite.checkSum1 = 0;
    packetWrite.checkSum2 = 0;
    packetWrite.bData = (char *)malloc(sizeof(char) * packetWrite.bLength + 1);
    memset(packetWrite.bData, '\0', sizeof(char) * packetWrite.bLength + 1);

    memset(packet, '\0', sizeof(char) * packetWrite.pSize);
    formatSendData(packet);

    softwareSerial->write(packet, strlen(packet));
    delay(100);
    hardwareSerial->println();
    hardwareSerial->println(packet);

    connection.setStatus(Connection::CONNECTED);
    return true;
  }

  connection.setStatus(Connection::DISCONNECTED);
  error.setError(Error::TCP_CONNECTION_ERROR);
  return false;
}

bool TcpProtocol::write(char *dataToSend) {
  if (connection.getStatus() == Connection::CONNECTED) {
    // sendData(dataToSend);
    return true;
  }
  error.setError(Error::WRITE_ERROR);
  return false;
}

void TcpProtocol::formatReceiveData(char *bData, char *dataToReceive) {
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
  packetRead.pKind = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.ack = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.checkSum1 = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.checkSum2 = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bData = pch;

  packetRead.bData[packetRead.bLength] = '\0';

  // if (!hasPacketErrors(packetRead.bData)) {
  //   strcat(dataToReceive, packetRead.bData);
  // } else {
  //   char err[100];
  //   snprintf(err, 100, "ERR_PACKET_%d_%d_[%s]", packetRead.checkSum1, packetRead.checkSum2, packetRead.bData);
  //   strcat(dataToReceive, err);
  // }
  strcat(dataToReceive, packetRead.bData);
}

void TcpProtocol::establishConnectionServer() {  // THREE WAY HANDSHAKE
  int bDataLength = TcpPacket::BLOCK_SIZE;
  char bData[bDataLength];
  char *dataToReceive;

  bool connectionEstablished = false;
  while (connectionEstablished == false) {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);
    formatReceiveData(bData, dataToReceive);

    if (packetRead.pKind == 1 && packetRead.ack == 0) {
      packetWrite.bNumber = 1;
      packetWrite.bOffset = 5001;
      packetWrite.pKind = 1;  // /ACK:0, SYN:1 FIN:2
      packetWrite.ack = packetRead.bOffset + 1;
      packetWrite.checkSum1 = 0;
      packetWrite.checkSum2 = 0;
      packetWrite.bData = (char *)malloc(sizeof(char) * packetWrite.bLength + 1);
      memset(packetWrite.bData, '\0', sizeof(char) * packetWrite.bLength + 1);

      char packet[packetWrite.pSize];
      memset(packet, '\0', sizeof(char) * packetWrite.pSize);
      formatSendData(packet);

      softwareSerial->write(packet, strlen(packet));
      delay(100);
      hardwareSerial->println();
      hardwareSerial->println(packet);

      waitRead();
      memset(bData, '\0', sizeof(char) * bDataLength);
      softwareSerial_readBytes(bData, bDataLength);
      formatReceiveData(bData, dataToReceive);

      if (packetRead.bOffset == packetWrite.bOffset && packetRead.ack == packetWrite.bOffset + 1 && packetRead.pKind == 0) {
        connectionEstablished = true;
        connection.setStatus(Connection::CONNECTED);
        softwareSerial->println("CONNECTION SERVER SIDE ESTABLISHED");
      } else {
        connection.setStatus(Connection::DISCONNECTED);
        error.setError(Error::TCP_CONNECTION_ERROR);
      }
    } else {
      packetWrite.bNumber = 1;
      packetWrite.bOffset = 0;
      packetWrite.pKind = 9999;  // /ACK:0, SYN:1 FIN:2
      packetWrite.ack = 0;
      packetWrite.checkSum1 = 0;
      packetWrite.checkSum2 = 0;
      packetWrite.bData = (char *)malloc(sizeof(char) * packetWrite.bLength + 1);
      memset(packetWrite.bData, '\0', sizeof(char) * packetWrite.bLength + 1);

      char packet[packetWrite.pSize];
      memset(packet, '\0', sizeof(char) * packetWrite.pSize);
      formatSendData(packet);

      softwareSerial->write(packet, strlen(packet));
      delay(100);
      hardwareSerial->println();
      hardwareSerial->println(packet);

      connection.setStatus(Connection::DISCONNECTED);
      error.setError(Error::TCP_CONNECTION_ERROR);
    }
  }
}

bool TcpProtocol::read(char *dataToReceive) {
  if (connection.getStatus() == Connection::CONNECTED) {
    // memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
    // receiveData(dataToReceive);
    return true;
  }
  error.setError(Error::READ_ERROR);

  return false;
}
