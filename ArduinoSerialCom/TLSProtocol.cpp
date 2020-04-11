#include "TLSProtocol.hpp"

#include "TcpPacket.hpp"

TLSProtocol::TLSProtocol() {
  masterKey[0] = '\0';
  useTLSProtocol = false;
  useMasterKey = false;
  primeProd = 143;
  publicKey = 7;
  privateKey = 103;
}

void TLSProtocol::setUseTLSProtocol(bool use) {
  useTLSProtocol = use;
}
bool TLSProtocol::getUseTLSProtocol() {
  return useTLSProtocol;
}

void TLSProtocol::setUseMasterKey(bool use) {
  useMasterKey = use;
}

bool TLSProtocol::getUseMasterKey() {
  return useMasterKey;
}

void TLSProtocol::generateRandomString(char *str) {
  randomSeed(analogRead(0));
  for (int index = 0; index < TcpPacket::BLOCK_BODY_SIZE - 1; index++) {
    str[index] = random(97, 122);
  }
}

void TLSProtocol::generateMasterKey(char *preMasterKey, char *randomFctString, char *masterKey) {
  for (int index = 0; index < TcpPacket::BLOCK_BODY_SIZE - 1; index++) {
    masterKey[index] = (preMasterKey[index] + randomFctString[index]) % 127;
  }
}

void TLSProtocol::encryptAES_CBC(char *data, char *result) {
  char *toEncrypt, *cypherText;
  char iv[17];
  int resultLength = strlen(data);
  int numberOfBlocks = resultLength / 16;
  if (resultLength % 16 != 0) {
    numberOfBlocks++;
  }

  toEncrypt = (char *)malloc(sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(toEncrypt, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  cypherText = (char *)malloc(sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(cypherText, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  strcpy(iv, masterKey);

  for (int index = 0; index < numberOfBlocks; index++) {
    memset(toEncrypt, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
    int k = 0;
    for (int encryptIndex = index * 16; encryptIndex < index * 16 + 16; encryptIndex++) {
      toEncrypt[k++] = data[encryptIndex];
    }  // formam blocurile de cate 16

    for (int i = 0; i < 16; i++) {
      cypherText[i] = iv[i] ^ toEncrypt[i];
    }
    // aes128_enc_single((const uint8_t *)masterKey, cypherText);

    strcat(result, cypherText);
    strcpy(iv, cypherText);
  }
  result[resultLength] = '\0';
  free(toEncrypt);
  free(cypherText);
}

void TLSProtocol::decryptAES_CBC(char *data, char *result) {
  char *toEncrypt, *cypherText;
  char iv[17];
  int resultLength = strlen(data);
  int numberOfBlocks = resultLength / 16;
  if (resultLength % 16 != 0) {
    numberOfBlocks++;
  }

  toEncrypt = (char *)malloc(sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(toEncrypt, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  cypherText = (char *)malloc(sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  memset(cypherText, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
  strcpy(iv, masterKey);

  for (int index = 0; index < numberOfBlocks; index++) {
    memset(toEncrypt, '\0', sizeof(char) * TcpPacket::BLOCK_BODY_SIZE);
    int k = 0;
    for (int encryptIndex = index * 16; encryptIndex < index * 16 + 16; encryptIndex++) {
      toEncrypt[k++] = data[encryptIndex];
    }  // formam blocurile de cate 16

    // aes128_dec_single((const uint8_t *)masterKey, toEncrypt);

    for (int i = 0; i < 16; i++) {
      cypherText[i] = iv[i] ^ toEncrypt[i];
    }
    strcat(result, cypherText);
    strcpy(iv, toEncrypt);
  }
  result[resultLength] = '\0';
  free(toEncrypt);
  free(cypherText);
}

void TLSProtocol::encryptRSA_decryptRSA(char *text, int key, char *result) {
  int offset = 0;
  int index;
  for (index = 0; index < strlen(text); index++) {
    int chr = text[index] - offset;
    long int pow = 1;
    for (int powIndex = 0; powIndex < key; powIndex++) {
      pow *= chr;
      pow %= primeProd;
    }
    // temp[index] = pow;
    result[index] = pow + offset;
  }
  result[index] = '\0';
}