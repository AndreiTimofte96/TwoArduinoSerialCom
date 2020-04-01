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
  addNumberToCharArray(packetWrite.UAID, packet);
  addOffsetToCharArray(packet, length);
  strcat(packet, packetWrite.bData);
}

void UdpProtocol::sendData(char *dataToSend, int UAID) {
  packetWrite.pSize = UdpPacket::BLOCK_SIZE;
  packetWrite.bLength = UdpPacket::BLOCK_BODY_SIZE;
  packetWrite.bNumber = strlen(dataToSend) / packetWrite.bLength;
  packetWrite.UAID = UAID;

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
    hardwareSerial->println();
    hardwareSerial->println(packet);

    free(packet);
    free(packetWrite.bData);
  }
}

bool UdpProtocol::write(char *dataToSend, int UAID) {
  if (connection.getStatus() == Connection::CONNECTED) {
    softwareSerial->listen();
    hardwareSerial->println("\nSENDING:");
    sendData(dataToSend, UAID);
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
  packetRead.UAID = atoi(pch);

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

void UdpProtocol::receiveData(char *dataToReceive, int &UAID) {
  int bDataLength = UdpPacket::BLOCK_SIZE;
  char *bData;
  bData = (char *)malloc(sizeof(char) * bDataLength + 1);

  do {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);
    hardwareSerial->println("READ:");
    hardwareSerial->println(bData);
    formatReceiveData(bData, dataToReceive);
  } while (packetRead.bOffset + 1 < packetRead.bNumber);

  UAID = packetRead.UAID;
  free(bData);
}

bool UdpProtocol::read(char *dataToReceive, int &UAID) {
  if (connection.getStatus() == Connection::CONNECTED) {
    softwareSerial->listen();
    hardwareSerial->println("\nRECEIVING:");
    memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
    receiveData(dataToReceive, UAID);
    return true;
  }
  error.setError(Error::READ_ERROR);

  return false;
}
