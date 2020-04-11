#include "TcpProtocol.hpp"

int ok = true;

TcpProtocol::TcpProtocol() {
  packetWrite.pSize = TcpPacket::BLOCK_SIZE;
  packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE;
  // connection.setStatus(Connection::CONNECTED);  // for debugging purposes
}

void TcpProtocol::useTLSProtocol() {
  hardwareSerial->println("USING TLS PROTOCOL");
  tlsProtocol.setUseTLSProtocol(true);
}

int TcpProtocol::listen() {  // THREE WAY HANDSHAKE
  softwareSerial->listen();
  int bDataLength = TcpConnection::BLOCK_SIZE;
  int packetLength = TcpConnection::BLOCK_SIZE;

  char *bData, *packet;
  bData = (char *)malloc(sizeof(char) * bDataLength + 1);
  packet = (char *)malloc(sizeof(char) * packetLength + 1);

  int clientUAID = -1;

  bool connectionEstablished = false;
  while (connectionEstablished == false) {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    softwareSerial_readBytes(bData, bDataLength);

    formatReceiveConnectionData(bData);

    clientUAID = packetConnectionRead.seq;

    if (packetConnectionRead.syn == 1 && packetConnectionRead.ack == 0) {
      packetConnectionWrite.seq = 5001;
      packetConnectionWrite.syn = 1;  // /ACK:0, SYN:1 FIN:2
      packetConnectionWrite.ack = packetConnectionRead.seq + 1;

      memset(packet, '\0', sizeof(char) * packetLength);
      formatSendConnectionData(packet, packetLength);

      softwareSerial->write(packet, strlen(packet));

      delay(10);

      waitRead();
      memset(bData, '\0', sizeof(char) * bDataLength);
      softwareSerial_readBytes(bData, bDataLength);

      formatReceiveConnectionData(bData);

      if (packetConnectionRead.seq == packetConnectionWrite.ack &&
          packetConnectionRead.ack == packetConnectionWrite.seq + 1) {
        connectionEstablished = true;
        connection.setStatus(Connection::CONNECTED);
        hardwareSerial->print("CONNECTION SERVER SIDE ESTABLISHED: ");
        hardwareSerial->println(clientUAID);

        /////// TLS CONNECTION ////////
        if (packetConnectionRead.syn == 4) {  // IF WE HAVE ALSO A TLS PROTOCOL
          tlsProtocol.setUseTLSProtocol(true);

          if (TLSListen() == false) {
            connectionEstablished = false;
            tlsProtocol.setUseTLSProtocol(false);

            connection.setStatus(Connection::DISCONNECTED);
            error.setError(Error::TCP_CONNECTION_ERROR);
          }
        }
        /////// TLS CONNECTION ////////

      } else {
        connection.setStatus(Connection::DISCONNECTED);
        error.setError(Error::TCP_CONNECTION_ERROR);
      }

    } else {
      packetConnectionWrite.seq = 1;
      packetConnectionWrite.syn = 9999;  // /ACK:0, SYN:1 CON:2 FIN:3 ERR: 9999
      packetConnectionWrite.ack = 0;

      memset(packet, '\0', sizeof(char) * packetLength);
      formatSendConnectionData(packet, packetLength);

      softwareSerial->write(packet, strlen(packet));

      delay(10);

      connection.setStatus(Connection::DISCONNECTED);
      error.setError(Error::TCP_CONNECTION_ERROR);
    }
  }

  free(bData);
  free(packet);

  return clientUAID;
}

bool TcpProtocol::connect() {  // THREE WAY HANDSHAKE
  int UAID = getUniqueArduinoIDFromEEEPROM();

  packetConnectionWrite.seq = UAID;
  packetConnectionWrite.ack = 0;
  packetConnectionWrite.syn = 1;  // /ACK:0, SEQ:1 CON:2 FIN:3

  int packetLength = TcpConnection::BLOCK_SIZE;
  char *packet;
  packet = (char *)malloc(sizeof(char) * packetLength + 1);

  memset(packet, '\0', sizeof(char) * packetLength + 1);
  formatSendConnectionData(packet, packetLength);

  softwareSerial->write(packet, strlen(packet));
  hardwareSerial->println(packet);

  delay(10);

  waitRead();
  int bDataLength = TcpConnection::BLOCK_SIZE;
  char *bData;
  bData = (char *)malloc(sizeof(char) * bDataLength + 1);

  memset(bData, '\0', sizeof(char) * bDataLength);

  softwareSerial_readBytes(bData, bDataLength);

  formatReceiveConnectionData(bData);

  if (packetConnectionRead.syn == 1 &&
      packetConnectionRead.ack == packetConnectionWrite.seq + 1) {
    packetConnectionWrite.seq = packetConnectionRead.ack;
    packetConnectionWrite.ack = packetConnectionRead.seq + 1;
    packetConnectionWrite.syn = tlsProtocol.getUseTLSProtocol() ? 4 : 0;  // /ACK:0, SEQ:1 CON:2 FIN:3 TLS: 4

    memset(packet, '\0', sizeof(char) * packetLength + 1);
    formatSendConnectionData(packet, packetLength);

    softwareSerial->write(packet, strlen(packet));

    free(bData);
    free(packet);
    delay(10);

    hardwareSerial->println("CONNECTION CLIENT SIDE ESTABLISHED");
    connection.setStatus(Connection::CONNECTED);

    ////// TLS CONNECTION //////
    if (tlsProtocol.getUseTLSProtocol()) {
      if (TLSConnect() == false) {
        connection.setStatus(Connection::DISCONNECTED);
        error.setError(Error::TCP_CONNECTION_ERROR);
        return false;
      }
    }
    ////// TLS CONNECTION //////

    return true;
  }

  free(bData);
  free(packet);
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

///////////////////////////////////TLS PROTOCOL//////////////////////////
bool TcpProtocol::TLSListen() {
  char randomFctString[17], publicKey[5], preMasterKey[17], preMasterKeyEncrypted[17];
  int UAID = 9999;

  tlsProtocol.masterKey = (char *)malloc(sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(tlsProtocol.masterKey, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);

  //STEP 1
  read(randomFctString, UAID);
  Serial.println(randomFctString);

  //STEP 2
  itoa(tlsProtocol.publicKey, publicKey, 10);
  write(publicKey, UAID);

  //STEP 3
  read(preMasterKeyEncrypted, UAID);

  //STEP 4
  tlsProtocol.encryptRSA_decryptRSA(preMasterKeyEncrypted, tlsProtocol.privateKey, preMasterKey);
  tlsProtocol.generateMasterKey(preMasterKey, randomFctString, tlsProtocol.masterKey);

  Serial.println("tlsProtocol.masterKey");
  Serial.println(tlsProtocol.masterKey);

  //STEP 6
  strcpy(randomFctString, "FIN");
  memset(preMasterKey, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(preMasterKeyEncrypted, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  read(preMasterKey, UAID);
  //encriptam FIN sosit de la client si-l punem in preMasterKeyEncrypted
  tlsProtocol.encryptAES_CBC(randomFctString, preMasterKeyEncrypted);

  bool ok = false;
  if (strcmp(preMasterKey, preMasterKeyEncrypted) == 0) {
    Serial.println("ESTABLISHED MASTER AES KEY");
    write(preMasterKeyEncrypted, UAID);
    ok = true;
    tlsProtocol.setUseMasterKey(ok);
  }
  return ok;
}

bool TcpProtocol::TLSConnect() {
  char randomFctString[17], publicKey[5], preMasterKey[17], preMasterKeyEncrypted[17];
  int UAID = 9999;

  tlsProtocol.masterKey = (char *)malloc(sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(tlsProtocol.masterKey, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);

  //STEP 1
  tlsProtocol.generateRandomString(randomFctString);
  write(randomFctString, UAID);

  //STEP 2
  read(publicKey, UAID);

  //STEP 3
  tlsProtocol.generateRandomString(preMasterKey);
  tlsProtocol.encryptRSA_decryptRSA(preMasterKey, atoi(publicKey), preMasterKeyEncrypted);
  write(preMasterKeyEncrypted, UAID);
  tlsProtocol.generateMasterKey(preMasterKey, randomFctString, tlsProtocol.masterKey);

  Serial.println("tlsProtocol.masterKey");
  Serial.println(tlsProtocol.masterKey);

  //STEP 5
  strcpy(randomFctString, "FIN");
  memset(preMasterKey, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(preMasterKeyEncrypted, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);

  tlsProtocol.encryptAES_CBC(randomFctString, preMasterKey);  // encrypt FIN
  write(preMasterKey, UAID);

  // //STEP 7
  read(preMasterKeyEncrypted, UAID);

  bool ok = false;
  if (strcmp(preMasterKey, preMasterKeyEncrypted) == 0) {
    Serial.println("ESTABLISHED MASTER AES KEY");
    ok = true;
    tlsProtocol.setUseMasterKey(ok);
  }
  return ok;
}
///////////////////////////////////TLS PROTOCOL//////////////////////////

void TcpProtocol::formatSendData(char *packet, int length, int checkSum1, int checkSum2) {
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

void TcpProtocol::sendData(char *dataToSend, int UAID) {
  packetWrite.pSize = TcpPacket::BLOCK_SIZE;
  packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE - 1;
  packetWrite.bNumber = strlen(dataToSend) / packetWrite.bLength;
  packetWrite.UAID = UAID;

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

    // TLS NOT WORKING;
    // if (tlsProtocol.getUseMasterKey()) {
    //   char encrypted[17];
    //   Serial.println("3333333");
    //   tlsProtocol.encryptAES_CBC(packetWrite.bData, encrypted);
    //   Serial.println(encrypted);
    //   Serial.println(strlen(encrypted));
    //   strcpy(packetWrite.bData, encrypted);
    // }
    // TLS NOT WORKING;

    if (blockIndex == packetWrite.bNumber - 1 && remainder) {
      packetWrite.bLength = remainder;
    } else {
      packetWrite.bLength = TcpPacket::BLOCK_BODY_SIZE;
    }

    char *packet;
    packet = (char *)malloc(sizeof(char) * packetWrite.pSize);
    memset(packet, '\0', sizeof(char) * packetWrite.pSize);
    formatSendData(packet, TcpPacket::BLOCK_HEADER_SIZE, checkSum1, checkSum2);

    SENT_ACK = false;
    while (SENT_ACK == false) {
      softwareSerial->write(packet, strlen(packet));
      delay(10);
      hardwareSerial->println();
      hardwareSerial->println(packet);

      waitRead();
      int bDataConnectionLength = TcpConnection::BLOCK_SIZE;
      char bDataConnection[bDataConnectionLength];
      memset(bDataConnection, '\0', sizeof(char) * bDataConnectionLength);

      softwareSerial_readBytes(bDataConnection, bDataConnectionLength);
      // hardwareSerial->println("CLIENT READ:");
      hardwareSerial->println(bDataConnection);
      formatReceiveConnectionData(bDataConnection);

      if (packetConnectionRead.ack == packetWrite.bOffset + 1 && packetConnectionRead.syn == 1) {
        SENT_ACK = true;
        free(packetWrite.bData);
      }
      packet[packetWrite.pSize] = '\0';  // BUG CIUDAT
    }
    free(packet);
  }
}

bool TcpProtocol::write(char *dataToSend, int UAID) {
  if (connection.getStatus() == Connection::CONNECTED) {
    softwareSerial->listen();
    hardwareSerial->println("\nSENDING:");
    sendData(dataToSend, UAID);
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
  packetRead.UAID = atoi(pch);

  pch = strtok(NULL, specialChr);
  packetRead.bData = pch;

  // TLS NOT WORKING;
  // if (tlsProtocol.getUseMasterKey()) {
  //   char decrypted[17];
  //   Serial.println("44444444");
  //   Serial.println(packetRead.bData);
  //   Serial.println(strlen(packetRead.bData));
  //   tlsProtocol.decryptAES_CBC(packetRead.bData, decrypted);
  //   Serial.println(decrypted);
  //   Serial.println(strlen(decrypted));
  //   strcpy(packetRead.bData, decrypted);
  // }
  // TLS NOT WORKING;

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

void TcpProtocol::receiveData(char *dataToReceive, int &UAID) {
  int bDataLength = TcpPacket::BLOCK_SIZE;
  char *bData;
  bData = (char *)malloc(sizeof(char) * bDataLength + 1);

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
    // hardwareSerial->println("CLIENT WRITE:");
    hardwareSerial->println(connectionPacket);
    delay(10);

  } while (packetRead.bOffset + 1 < packetRead.bNumber);

  free(connectionPacket);
  free(bData);

  for (int index = 0; index < packetRead.bNumber; index++) {
    strcat(dataToReceive, orderedPackets[index]);
    free(orderedPackets[index]);
  }
  free(orderedPackets);

  UAID = packetRead.UAID;
}

bool TcpProtocol::read(char *dataToReceive, int &UAID) {
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
