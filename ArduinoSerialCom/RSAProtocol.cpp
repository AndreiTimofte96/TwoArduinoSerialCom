#include "RSAProtocol.hpp"

RSAProtocol::RSAProtocol() {
  primeProd = 143;
  publicKey = 7;
  privateKey = 103;
}

void RSAProtocol::encrypt_decrypt(char* text, char* result, int key) {
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