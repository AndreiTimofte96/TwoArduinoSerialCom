#include "TcpProtocol.hpp"

TcpProtocol::TcpProtocol() {
  packetWrite.pSize = TcpPacket::BLOCK_SIZE;
  packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE;
}

void TcpProtocol::listen() {  // THREE WAY HANDSHAKE
  int bDataLength = TcpConnection::BLOCK_SIZE;
  char bData[bDataLength];

  bool connectionEstablished = false;
  while (connectionEstablished == false) {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);
    // hardwareSerial->println("SERVER READ:");
    // hardwareSerial->println(bData);

    formatReceiveConnectionData(bData);

    if (packetConnectionRead.syn == 1 && packetConnectionRead.ack == 0) {
      packetConnectionWrite.seq = 5001;
      packetConnectionWrite.syn = 1;  // /ACK:0, SYN:1 FIN:2
      packetConnectionWrite.ack = packetConnectionRead.seq + 1;

      int packetLength = TcpConnection::BLOCK_SIZE;
      char packet[packetLength];
      memset(packet, '\0', sizeof(char) * packetLength);
      formatSendConnectionData(packet, packetLength);

      softwareSerial->write(packet, strlen(packet));
      delay(100);
      // hardwareSerial->println("SERVER WRITE:");
      // hardwareSerial->println(packet);

      waitRead();
      memset(bData, '\0', sizeof(char) * bDataLength);
      softwareSerial_readBytes(bData, bDataLength);
      // hardwareSerial->println("SERVER READ:");
      // hardwareSerial->println(bData);

      formatReceiveConnectionData(bData);

      if (packetConnectionRead.seq == packetConnectionWrite.ack && packetConnectionRead.ack == packetConnectionWrite.seq + 1 && packetConnectionRead.syn == 0) {
        connectionEstablished = true;
        connection.setStatus(Connection::CONNECTED);
        hardwareSerial->println("CONNECTION SERVER SIDE ESTABLISHED");
      } else {
        connection.setStatus(Connection::DISCONNECTED);
        error.setError(Error::TCP_CONNECTION_ERROR);
      }
    } else {
      packetConnectionWrite.seq = 1;
      packetConnectionWrite.syn = 9999;  // /ACK:0, SYN:1 FIN:2 ERR: 9999
      packetConnectionWrite.ack = 0;

      int packetLength = TcpConnection::BLOCK_SIZE;
      char packet[packetLength];
      memset(packet, '\0', sizeof(char) * packetLength);
      formatSendConnectionData(packet, packetLength);

      softwareSerial->write(packet, strlen(packet));
      delay(100);
      // hardwareSerial->println("SERVER WRITE:");
      // hardwareSerial->println(packet);

      connection.setStatus(Connection::DISCONNECTED);
      error.setError(Error::TCP_CONNECTION_ERROR);
    }
  }
}

bool TcpProtocol::connect() {  // THREE WAY HANDSHAKE

  packetConnectionWrite.seq = 9001;
  packetConnectionWrite.ack = 0;
  packetConnectionWrite.syn = 1;  // /ACK:0, SEQ:1 CON:2 FIN:3

  int packetLength = TcpConnection::BLOCK_SIZE;
  char *packet;
  packet = (char *)malloc(sizeof(char) * packetLength + 1);

  memset(packet, '\0', sizeof(char) * packetLength + 1);
  formatSendConnectionData(packet, packetLength);

  softwareSerial->write(packet, strlen(packet));
  // hardwareSerial->println("CLIENT WRITE:");
  // hardwareSerial->println(packet);
  delay(100);

  waitRead();
  int bDataLength = TcpConnection::BLOCK_SIZE;
  char bData[bDataLength];
  memset(bData, '\0', sizeof(char) * bDataLength);

  softwareSerial_readBytes(bData, bDataLength);
  // hardwareSerial->println("CLIENT READ:");
  // hardwareSerial->println(bData);

  formatReceiveConnectionData(bData);

  if (packetConnectionRead.syn == 1 && packetConnectionRead.ack == packetConnectionWrite.seq + 1) {
    packetConnectionWrite.seq = packetConnectionRead.ack;
    packetConnectionWrite.ack = packetConnectionRead.seq + 1;
    packetConnectionWrite.syn = 0;  // /ACK:0, SEQ:1 CON:2 FIN:3

    memset(packet, '\0', sizeof(char) * packetLength + 1);
    formatSendConnectionData(packet, packetLength);

    softwareSerial->write(packet, strlen(packet));
    // hardwareSerial->println("CLIENT WRITE:");
    // hardwareSerial->println(packet);
    delay(100);

    hardwareSerial->println("CONNECTION CLIENT SIDE ESTABLISHED");
    connection.setStatus(Connection::CONNECTED);
    return true;
  }

  connection.setStatus(Connection::DISCONNECTED);
  error.setError(Error::TCP_CONNECTION_ERROR);
  return false;
}

void TcpProtocol::formatReceiveConnectionData(char *bData) {
  char *pch;

  pch = strtok(bData, specialChr);
  packetConnectionRead.seq = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetConnectionRead.ack = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetConnectionRead.syn = atoi(pch);
}

void TcpProtocol::formatSendConnectionData(char *packet, int packetLength) {
  addNumberToCharArray(packetConnectionWrite.seq, packet);
  addNumberToCharArray(packetConnectionWrite.ack, packet);
  addNumberToCharArray(packetConnectionWrite.syn, packet);

  addOffsetToCharArray(packet, packetLength);
}

/////////////////////////////////////////////////////////////////////////////////////

void TcpProtocol::formatSendData(char *packet, int length) {
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

void TcpProtocol::sendData(char *dataToSend) {
  packetWrite.pSize = TcpPacket::BLOCK_SIZE;
  packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE;
  packetWrite.bNumber = strlen(dataToSend) / packetWrite.bLength;
  if (strlen(dataToSend) % packetWrite.bLength) {
    packetWrite.bNumber += 1;
  }
  int remainder = strlen(dataToSend) % packetWrite.bLength;
  int bLength = packetWrite.bLength;
  int blockLength = bLength;
  bool SENT_ACK = false;

  for (int blockIndex = 0; blockIndex < packetWrite.bNumber; blockIndex++) {
    if (blockIndex == packetWrite.bNumber - 1) {
      packetWrite.bLength = remainder;
      blockLength = remainder;
      // hardwareSerial->println(remainder);  // BUG DUBIOS
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
    formatSendData(packet, TcpPacket::BLOCK_HEADER_SIZE);

    SENT_ACK = false;
    while (SENT_ACK == false) {
      softwareSerial->write(packet, strlen(packet));
      delay(100);
      hardwareSerial->println();
      hardwareSerial->println(packet);

      waitRead();
      int bDataLength = TcpConnection::BLOCK_SIZE;
      char bData[bDataLength];
      memset(bData, '\0', sizeof(char) * bDataLength);

      softwareSerial_readBytes(bData, bDataLength);
      hardwareSerial->println("CLIENT READ:");
      hardwareSerial->println(bData);
      formatReceiveConnectionData(bData);

      if (packetConnectionRead.ack == packetWrite.bOffset + 1) {
        SENT_ACK = true;
      }
    }
  }
}

bool TcpProtocol::write(char *dataToSend) {
  if (connection.getStatus() == Connection::CONNECTED) {
    sendData(dataToSend);
    return true;
  }
  error.setError(Error::WRITE_ERROR);
  return false;
}

/////////////////////////////////////////////////////////////

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
  packetRead.checkSum1 = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.checkSum2 = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bData = pch;

  packetRead.bData[packetRead.bLength] = '\0';
  int blockLength = TcpPacket::BLOCK_BODY_SIZE;
  if (packetRead.bOffset == 0) {
    orderedPackets = (char **)malloc(sizeof(char *) * packetRead.bLength);
    for (int index = 0; index < packetRead.bLength; index++) {
      orderedPackets[index] = (char *)malloc(sizeof(char *) * blockLength);
    }
  }
  memset(orderedPackets[packetRead.bOffset], '\0', sizeof(char) * blockLength);
  strcpy(orderedPackets[packetRead.bOffset], packetRead.bData);
}

void TcpProtocol::receiveData(char *dataToReceive) {
  int bDataLength = TcpPacket::BLOCK_SIZE;
  char bData[bDataLength];
  int packetLength = TcpConnection::BLOCK_SIZE;
  char *packet;
  packet = (char *)malloc(sizeof(char) * packetLength + 1);

  do {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);
    hardwareSerial->println("READ:");
    hardwareSerial->println(bData);

    formatReceiveData(bData, dataToReceive);

    packetConnectionWrite.seq = 1;
    packetConnectionWrite.ack = packetRead.bOffset + 1;
    packetConnectionWrite.syn = 1;  // /ACK:0, SEQ:1 CON:2 FIN:3

    memset(packet, '\0', sizeof(char) * packetLength + 1);
    formatSendConnectionData(packet, packetLength);

    softwareSerial->write(packet, strlen(packet));
    hardwareSerial->println("CLIENT WRITE:");
    hardwareSerial->println(packet);
    delay(100);

  } while (packetRead.bOffset + 1 < packetRead.bNumber);

  for (int index = 0; index < packetRead.bNumber; index++) {
    strcat(dataToReceive, orderedPackets[index]);
  }
}

bool TcpProtocol::read(char *dataToReceive) {
  if (connection.getStatus() == Connection::CONNECTED) {
    memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
    receiveData(dataToReceive);
    return true;
  }
  error.setError(Error::READ_ERROR);

  return false;
}
