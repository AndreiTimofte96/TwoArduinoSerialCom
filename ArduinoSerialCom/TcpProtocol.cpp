#include "TcpProtocol.hpp"

int ok = true;

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

      waitRead();
      memset(bData, '\0', sizeof(char) * bDataLength);
      softwareSerial_readBytes(bData, bDataLength);

      formatReceiveConnectionData(bData);

      if (packetConnectionRead.seq == packetConnectionWrite.ack &&
          packetConnectionRead.ack == packetConnectionWrite.seq + 1 && packetConnectionRead.syn == 0) {
        connectionEstablished = true;
        connection.setStatus(Connection::CONNECTED);
        hardwareSerial->println("CONNECTION SERVER SIDE ESTABLISHED");
      } else {
        connection.setStatus(Connection::DISCONNECTED);
        error.setError(Error::TCP_CONNECTION_ERROR);
      }
    } else {
      packetConnectionWrite.seq = 1;
      packetConnectionWrite.syn = 9999;  // /ACK:0, SYN:1 CON:2 FIN:3 ERR: 9999
      packetConnectionWrite.ack = 0;

      int packetLength = TcpConnection::BLOCK_SIZE;
      char packet[packetLength];
      memset(packet, '\0', sizeof(char) * packetLength);
      formatSendConnectionData(packet, packetLength);

      softwareSerial->write(packet, strlen(packet));
      delay(100);

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

  delay(100);

  waitRead();
  int bDataLength = TcpConnection::BLOCK_SIZE;
  char bData[bDataLength];
  memset(bData, '\0', sizeof(char) * bDataLength);

  softwareSerial_readBytes(bData, bDataLength);

  formatReceiveConnectionData(bData);

  if (packetConnectionRead.syn == 1 &&
      packetConnectionRead.ack == packetConnectionWrite.seq + 1) {
    packetConnectionWrite.seq = packetConnectionRead.ack;
    packetConnectionWrite.ack = packetConnectionRead.seq + 1;
    packetConnectionWrite.syn = 0;  // /ACK:0, SEQ:1 CON:2 FIN:3

    memset(packet, '\0', sizeof(char) * packetLength + 1);
    formatSendConnectionData(packet, packetLength);

    softwareSerial->write(packet, strlen(packet));
    free(packet);
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

void TcpProtocol::formatSendData(char *packet, int length, int checkSum1, int checkSum2) {
  addNumberToCharArray(packetWrite.pSize, packet);
  addNumberToCharArray(packetWrite.bLength, packet);
  addNumberToCharArray(packetWrite.bNumber, packet);
  addNumberToCharArray(packetWrite.bOffset, packet);
  addNumberToCharArray(checkSum1, packet);
  addNumberToCharArray(checkSum2, packet);
  addOffsetToCharArray(packet, length);
  strcat(packet, packetWrite.bData);
}

////////////////////// HAMMMING DISTANCE CODE ENCODING METHOD ////////////
bool TcpProtocol::isPowerOfTwo(int number) {
  if (number == 0)
    return false;
  double log2N = log(number) / log(2);
  return (ceil(log2N) == floor(log2N));
}

void TcpProtocol::baseTwoToChar(int *bits, int length, char *str) {
  int strIndex = 0;
  for (int index = 1; index <= length;) {
    int number = 0;
    for (int bitIndex = 7; bitIndex >= 0; bitIndex--) {
      if (bits[index] == 1) {
        number += 1 << bitIndex;
      }
      index++;
    }
    str[strIndex++] = number;
  }
  // str[strlen(str) + 1] = '\n';
}

void TcpProtocol::charToBaseTwo(char *str, int *bits) {
  int bitIndex = 1;
  for (int charIndex = 0; charIndex < strlen(str); charIndex++) {
    char c = str[charIndex];

    //compute current character in base 2 and add it to str
    for (int i = 7; i >= 0; i--, bitIndex++) {
      bits[bitIndex] = (c & (1 << i)) ? 1 : 0;
    }
  }
}

void TcpProtocol::encodeWitHammingDistanceCode(char *dataSendString) {
  int dataSendBits[140];  // 128 bits  + 8 parityBits => 136 bits -> fix 17 bytes
  int dataSendLength = TcpPacket::BLOCK_BODY_SIZE - 1;
  int parityBits = 8;
  int dataSendBitsLength = parityBits + dataSendLength * 8;

  //fill parity bits position with -1 and the rest with 0
  for (int index = 1; index <= dataSendLength * 8 + parityBits; index++) {
    if (isPowerOfTwo(index)) {
      dataSendBits[index] = -1;
    } else {
      dataSendBits[index] = 0;
    }
  }

  int dataBitsIndex = 1;
  for (int charIndex = 0; charIndex < strlen(dataSendString); charIndex++) {
    char c = dataSendString[charIndex];

    //compute current character in base 2 and add it to dataSendBits
    for (int i = 7; i >= 0; dataBitsIndex++) {
      if (dataSendBits[dataBitsIndex] != -1) {
        dataSendBits[dataBitsIndex] = (c & (1 << i)) ? 1 : 0;
        i--;
      }
    }
  }

  int paritySum;
  int skipFactor;
  //compute parityBits value by Hamming Distance Code algorithm
  for (int index = 1; index <= dataSendBitsLength; index++) {
    if (dataSendBits[index] == -1) {
      skipFactor = index - 1;
      paritySum = 0;

      for (int jindex = index + 1; jindex <= dataSendBitsLength;) {
        if (skipFactor) {
          skipFactor--;
          paritySum += dataSendBits[jindex];
          jindex++;
        } else {
          jindex += index;
          skipFactor = index;
        }
      }
      dataSendBits[index] = paritySum % 2;
    }
  }

  dataSendEncodedString = (char *)malloc(sizeof(char) * packetWrite.bLength + 1 + 1);
  memset(dataSendEncodedString, '\0', sizeof(char) * packetWrite.bLength + 1 + 1);

  baseTwoToChar(dataSendBits, dataSendBitsLength, dataSendEncodedString);

  strcpy(dataSendString, dataSendEncodedString);

  free(dataSendEncodedString);
}

void TcpProtocol::decodeWitHammingDistanceCode(char *dataSendEncodedString) {
  int dataSendEncodedBits[140];
  int dataSendLength = TcpPacket::BLOCK_BODY_SIZE - 1;
  int parityBits = 8;
  int dataSendBitsLength = parityBits + dataSendLength * 8;

  for (int index = 0; index < 140; index++) {
    dataSendEncodedBits[index] = 0;
  }

  charToBaseTwo(dataSendEncodedString, dataSendEncodedBits);

  // TO SEE IF THE ERROR HANDLING MECHANISM REALLY WORKS
  // if (packetRead.bOffset == 2 && ok) {
  //   int _wrongBit = 27;
  //   ok = false;
  //   dataSendEncodedBits[_wrongBit] = !dataSendEncodedBits[_wrongBit];
  //   dataSendEncodedBits[_wrongBit + 3] = !dataSendEncodedBits[_wrongBit + 3];
  // }

  int wrongBit = 0;
  for (int index = 1; index <= dataSendBitsLength; index++) {
    if (isPowerOfTwo(index)) {
      int skipFactor = index - 1;
      int paritySum = 0;
      for (int jindex = index + 1; jindex <= dataSendBitsLength;) {
        if (skipFactor) {
          skipFactor--;
          paritySum += dataSendEncodedBits[jindex];
          jindex++;
        } else {
          jindex += index;
          skipFactor = index;
        }
      }

      if (paritySum % 2 != dataSendEncodedBits[index]) {
        wrongBit += index;
      }
    }
  }

  if (wrongBit) {
    dataSendEncodedBits[wrongBit] = !dataSendEncodedBits[wrongBit];
    hardwareSerial->println("WRONG BIT CORRECTED!");
  }

  int dataReceiveBits[140];
  int dataBitsReceiveIndex = 1;
  for (int index = 1; index <= dataSendBitsLength; index++) {
    if (!isPowerOfTwo(index)) {
      dataReceiveBits[dataBitsReceiveIndex++] = dataSendEncodedBits[index];
    }
  }
  baseTwoToChar(dataReceiveBits, dataSendLength * 8, dataSendEncodedString);
}

////////////////////// HAMMMING DISTANCE CODE ENCODING METHOD ////////////

void TcpProtocol::sendData(char *dataToSend) {
  packetWrite.pSize = TcpPacket::BLOCK_SIZE;
  packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE - 1;
  packetWrite.bNumber = strlen(dataToSend) / packetWrite.bLength;

  int remainder = strlen(dataToSend) % packetWrite.bLength;
  if (remainder) {
    packetWrite.bNumber += 1;
    remainder += 1;  //adding the parityBits -> 1 byte
  }
  int bLengthCopy = packetWrite.bLength;
  int currentOffset = bLengthCopy;
  bool SENT_ACK = false;

  for (int blockIndex = 0; blockIndex < packetWrite.bNumber; blockIndex++) {
    packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE - 1;
    if (blockIndex == packetWrite.bNumber - 1 && remainder) {
      packetWrite.bLength = remainder;
      currentOffset = remainder;
      // hardwareSerial->println(remainder);  // BUG DUBIOS
    }
    packetWrite.bOffset = blockIndex;
    packetWrite.bData = (char *)malloc(sizeof(char) * packetWrite.bLength + 1 + 1);
    memset(packetWrite.bData, '\0', sizeof(char) * packetWrite.bLength + 1 + 1);

    for (int dataIndex = blockIndex * bLengthCopy; dataIndex < blockIndex * bLengthCopy + currentOffset; dataIndex++) {
      int bDataLength = strlen(packetWrite.bData);

      packetWrite.bData[bDataLength] = dataToSend[dataIndex];
    }

    //computeCheckSums
    int checkSum1 = 0, checkSum2 = 0;
    computeChecksum(packetWrite.bData, checkSum1, checkSum2);
    /////

    //encode data to send -> packetWrite.bData;
    hardwareSerial->println(packetWrite.bData);
    encodeWitHammingDistanceCode(packetWrite.bData);
    hardwareSerial->println(packetWrite.bData);

    if (blockIndex == packetWrite.bNumber - 1 && remainder) {
      packetWrite.bLength = remainder;
    } else {
      packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE;
    }

    char packet[packetWrite.pSize];
    memset(packet, '\0', sizeof(char) * packetWrite.pSize);
    formatSendData(packet, TcpPacket::BLOCK_HEADER_SIZE, checkSum1, checkSum2);

    SENT_ACK = false;
    while (SENT_ACK == false) {
      softwareSerial->write(packet, strlen(packet));
      delay(100);
      hardwareSerial->println();
      hardwareSerial->println(packet);

      waitRead();
      int bDataConnectionLength = TcpConnection::BLOCK_SIZE;
      char bDataConnection[bDataConnectionLength];
      memset(bDataConnection, '\0', sizeof(char) * bDataConnectionLength);

      softwareSerial_readBytes(bDataConnection, bDataConnectionLength);
      hardwareSerial->println("CLIENT READ:");
      hardwareSerial->println(bDataConnection);
      formatReceiveConnectionData(bDataConnection);

      if (packetConnectionRead.ack == packetWrite.bOffset + 1 && packetConnectionRead.syn == 1) {
        SENT_ACK = true;
        free(packetWrite.bData);
      }
      packet[packetWrite.pSize] = '\0';  // BUG CIUDAT
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

bool TcpProtocol::formatReceiveData(char *bData) {
  char *pch;
  pch = strtok(bData, specialChr);
  packetRead.pSize = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bLength = atoi(pch) - 1;  // removing the parityBits

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

  decodeWitHammingDistanceCode(packetRead.bData);
  packetRead.bData[packetRead.bLength] = '\0';

  int checkSum1 = 0, checkSum2 = 0;
  computeChecksum(packetRead.bData, checkSum1, checkSum2);
  if (checkSum1 != packetRead.checkSum1 || checkSum2 != packetRead.checkSum2) {
    hardwareSerial->println("ERROR! RESEND PACKET");
    return false;
  }

  if (packetRead.bOffset == 0) {
    orderedPackets = (char **)malloc(sizeof(char *) * packetRead.bNumber);
    for (int index = 0; index < packetRead.bNumber; index++) {
      orderedPackets[index] = (char *)malloc(sizeof(char *) * packetRead.bLength + 2);
      memset(orderedPackets[index], '\0', sizeof(char) * packetRead.bLength + 2);
    }
  }

  strcpy(orderedPackets[packetRead.bOffset], packetRead.bData);
  hardwareSerial->println(orderedPackets[packetRead.bOffset]);
  return true;
}

void TcpProtocol::receiveData(char *dataToReceive) {
  int bDataLength = TcpPacket::BLOCK_SIZE;
  char bData[bDataLength];
  int packetLength = TcpConnection::BLOCK_SIZE;
  char *connectionPacket;
  connectionPacket = (char *)malloc(sizeof(char) * packetLength + 1);

  do {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);
    hardwareSerial->println("READ:");
    hardwareSerial->println(bData);

    bool isPacketOk = formatReceiveData(bData);

    packetConnectionWrite.seq = 1;
    packetConnectionWrite.ack = packetRead.bOffset + 1;
    packetConnectionWrite.syn = isPacketOk ? 1 : 9;  // /ACK:0, SYN:1 CON:2 FIN:3 ERR: 9999

    memset(connectionPacket, '\0', sizeof(char) * packetLength + 1);
    formatSendConnectionData(connectionPacket, packetLength);

    softwareSerial->write(connectionPacket, strlen(connectionPacket));
    hardwareSerial->println("CLIENT WRITE:");
    hardwareSerial->println(connectionPacket);
    delay(100);

  } while (packetRead.bOffset + 1 < packetRead.bNumber);

  free(connectionPacket);

  for (int index = 0; index < packetRead.bNumber; index++) {
    strcat(dataToReceive, orderedPackets[index]);
    free(orderedPackets[index]);
  }
  free(orderedPackets);
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
