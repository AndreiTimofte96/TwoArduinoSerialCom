#include "UdpProtocol.hpp"

UdpProtocol::UdpProtocol() {
  connection.setStatus(Connection::DISCONNECTED);
  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
}

int UdpProtocol::connect() {
  if (connection.getStatus() == Connection::ERROR) {
    whileForever();
    return Error::ERROR;
  }
  if (connection.getStatus() == Connection::FINISHED) {
    error.setError(Error::CONNECT_PROTOCOL_PROTOCOL_HAS_FINISHED);
    return Error::FINISHED;
  }
  if (connection.getStatus() == Connection::CONNECTED) {
    return Error::CONNECTED;
  }
  connection.setStatus(Connection::CONNECTED);
  return Error::CONNECTED;
}

void UdpProtocol::formatSendData(char *packet, int length) {
  int checkSum1 = packetWrite.checkSum1;
  int checkSum2 = packetWrite.checkSum2;

  computeChecksum(packetWrite.bData, checkSum1, checkSum2);

  addNumberToCharArray(packetWrite.pSize, packet);
  addNumberToCharArray(packetWrite.bLength, packet);
  addNumberToCharArray(packetWrite.bNumber, packet);
  addNumberToCharArray(packetWrite.bOffset, packet);
  addNumberToCharArray(checkSum1, packet);
  addNumberToCharArray(checkSum2, packet);
  addNumberToCharArray(packetWrite.fromUAID, packet);
  addNumberToCharArray(packetWrite.toUAID, packet);

  addOffsetToCharArray(packet, length);
  strcat(packet, packetWrite.bData);
}

int UdpProtocol::sendData(char *dataToSend, int fromUAID, int toUAID) {
  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
  packetWrite.bNumber = strlen(dataToSend) / packetWrite.bLength;
  packetWrite.fromUAID = fromUAID == -1 ? getUniqueArduinoIDFromEEEPROM() : fromUAID;
  packetWrite.toUAID = toUAID;

  int remainder = strlen(dataToSend) % packetWrite.bLength;
  if (remainder) {
    packetWrite.bNumber += 1;
  }
  int bLengthCopy = packetWrite.bLength;
  int currentOffset = bLengthCopy;
  for (int blockIndex = 0; blockIndex < packetWrite.bNumber; blockIndex++) {
    if (blockIndex == packetWrite.bNumber - 1 && remainder) {
      packetWrite.bLength = remainder;
      currentOffset = remainder;
    }
    packetWrite.bOffset = blockIndex;
    packetWrite.bData = (char *)malloc(sizeof(char) * packetWrite.bLength + 1);
    memset(packetWrite.bData, '\0', sizeof(char) * packetWrite.bLength + 1);

    for (int dataIndex = blockIndex * bLengthCopy; dataIndex < blockIndex * bLengthCopy + currentOffset; dataIndex++) {
      int bDataLength = strlen(packetWrite.bData);

      packetWrite.bData[bDataLength] = dataToSend[dataIndex];
    }
    char *packet;
    packet = (char *)malloc(sizeof(char) * packetWrite.pSize);
    memset(packet, '\0', sizeof(char) * packetWrite.pSize);
    formatSendData(packet, UdpPacket::BLOCK_HEADER_SIZE);

    softwareSerial->write(packet, strlen(packet));
    delay(10);
    hardwareSerial->println(F("WRITE:"));
    hardwareSerial->println(packet);

    free(packet);
    free(packetWrite.bData);
  }

  return strlen(dataToSend);
}

int UdpProtocol::write(char *dataToSend, int toUAID) {
  if (connection.getStatus() == Connection::ERROR) {
    whileForever();
    return Error::ERROR;
  }
  if (connection.getStatus() == Connection::DISCONNECTED) {
    error.setError(Error::WRITE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::DISCONNECTED;
  }
  if (connection.getStatus() == Connection::FINISHED) {
    error.setError(Error::WRITE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::FINISHED;
  }

  softwareSerial->listen();
  hardwareSerial->println(F("\nSENDING:"));
  return sendData(dataToSend, -1, toUAID);
}

int UdpProtocol::write(char *dataToSend, int fromUAID, int toUAID) {
  if (connection.getStatus() == Connection::ERROR) {
    whileForever();
    return Error::ERROR;
  }
  if (connection.getStatus() == Connection::DISCONNECTED) {
    error.setError(Error::WRITE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::DISCONNECTED;
  }
  if (connection.getStatus() == Connection::FINISHED) {
    error.setError(Error::WRITE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::FINISHED;
  }

  softwareSerial->listen();
  hardwareSerial->println(F("\nSENDING:"));
  return sendData(dataToSend, fromUAID, toUAID);
}

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
  packetRead.fromUAID = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.toUAID = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bData = pch;

  packetRead.bData[packetRead.bLength] = '\0';

  if (!hasPacketErrors(packetRead.bData, packetRead.checkSum1, packetRead.checkSum2)) {
    strcat(dataToReceive, packetRead.bData);
  } else {
    char err[100];  // TO BE TRANSFORMED IN DYNAMIC MEM ALLOCATION
    snprintf(err, 100, "ERR_PACKET_%d_%d_[%s]", packetRead.checkSum1, packetRead.checkSum2, packetRead.bData);
    strcat(dataToReceive, err);
  }
}

int UdpProtocol::receiveData(char *dataToReceive, int &fromUAID, int &toUAID) {
  int bDataLength = UdpPacket::BLOCK_SIZE;
  char *bData;
  bData = (char *)malloc(sizeof(char) * bDataLength + 1);

  do {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);
    hardwareSerial->println(F("READ:"));
    hardwareSerial->println(bData);
    formatReceiveData(bData, dataToReceive);
  } while (packetRead.bOffset + 1 < packetRead.bNumber);

  fromUAID = packetRead.fromUAID;
  toUAID = packetRead.toUAID;
  free(bData);

  return strlen(dataToReceive);
}

int UdpProtocol::read(char *dataToReceive, int &fromUAID) {
  if (connection.getStatus() == Connection::ERROR) {
    whileForever();
    return Error::ERROR;
  }
  if (connection.getStatus() == Connection::DISCONNECTED) {
    error.setError(Error::READ_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::DISCONNECTED;
  }
  if (connection.getStatus() == Connection::FINISHED) {
    error.setError(Error::READ_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::FINISHED;
  }

  softwareSerial->listen();
  hardwareSerial->println(F("\nRECEIVING:"));
  memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
  int toUAID;
  return receiveData(dataToReceive, fromUAID, toUAID);
}

int UdpProtocol::read(char *dataToReceive, int &fromUAID, int &toUAID) {
  if (connection.getStatus() == Connection::ERROR) {
    whileForever();
    return Error::ERROR;
  }
  if (connection.getStatus() == Connection::DISCONNECTED) {
    error.setError(Error::READ_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::DISCONNECTED;
  }
  if (connection.getStatus() == Connection::FINISHED) {
    error.setError(Error::READ_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::FINISHED;
  }

  softwareSerial->listen();
  hardwareSerial->println(F("\nRECEIVING:"));
  memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
  return receiveData(dataToReceive, fromUAID, toUAID);
}

int UdpProtocol::serverClose() {
  if (connection.getStatus() == Connection::ERROR) {
    whileForever();
    return Error::ERROR;
  }
  if (connection.getStatus() == Connection::DISCONNECTED) {
    error.setError(Error::CLOSE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::DISCONNECTED;
  }
  if (connection.getStatus() == Connection::FINISHED) {
    error.setError(Error::CLOSE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::FINISHED;
  }

  hardwareSerial->println(F("SERVER CLOSE:"));
  connection.setStatus(Connection::FINISHED);
  hardwareSerial->println(F("FIN Server Connection"));
}

int UdpProtocol::clientClose() {
  if (connection.getStatus() == Connection::ERROR) {
    whileForever();
    return Error::ERROR;
  }
  if (connection.getStatus() == Connection::DISCONNECTED) {
    error.setError(Error::CLOSE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::DISCONNECTED;
  }
  if (connection.getStatus() == Connection::FINISHED) {
    error.setError(Error::CLOSE_PROTOCOL_NOT_CONNECTED_ERROR);
    return Error::FINISHED;
  }

  hardwareSerial->println(F("CLIENT CLOSE:"));
  connection.setStatus(Connection::FINISHED);
  hardwareSerial->println(F("FIN Client Connection"));
  return Error::CLOSED_CONN;
}
