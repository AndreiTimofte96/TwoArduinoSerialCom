#include "UdpProtocol.hpp"

UdpProtocol::UdpProtocol() {
  connection.setStatus(Connection::CONNECTED);
  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
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
  addOffsetToCharArray(packet, length);
  strcat(packet, packetWrite.bData);
}

void UdpProtocol::sendData(char *dataToSend) {
  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
  packetWrite.bNumber = strlen(dataToSend) / packetWrite.bLength;
  if (strlen(dataToSend) % packetWrite.bLength) {
    packetWrite.bNumber += 1;
  }
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
    formatSendData(packet, UdpPacket::BLOCK_HEADER_SIZE);

    softwareSerial->write(packet, strlen(packet));
    delay(100);
    hardwareSerial->println();
    hardwareSerial->println(packet);
  }
}

bool UdpProtocol::write(char *dataToSend) {
  if (connection.getStatus() == Connection::CONNECTED) {
    sendData(dataToSend);
    return true;
  }
  error.setError(Error::WRITE_ERROR);
  return false;
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
  packetRead.bData = pch;

  packetRead.bData[packetRead.bLength] = '\0';

  if (!hasPacketErrors(packetRead.bData, packetRead.checkSum1, packetRead.checkSum2)) {
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

  waitRead();
  memset(bData, '\0', sizeof(char) * bDataLength);
  softwareSerial_readBytes(bData, bDataLength);
  formatReceiveData(bData, dataToReceive);

  while (packetRead.bOffset + 1 < packetRead.bNumber) {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);
    formatReceiveData(bData, dataToReceive);
  }
}

bool UdpProtocol::read(char *dataToReceive) {
  if (connection.getStatus() == Connection::CONNECTED) {
    memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
    receiveData(dataToReceive);
    return true;
  }
  error.setError(Error::READ_ERROR);

  return false;
}
