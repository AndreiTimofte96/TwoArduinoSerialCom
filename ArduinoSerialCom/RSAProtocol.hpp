#include <Arduino.h>

class RSAProtocol {
 public:
  int primeProd, publicKey, privateKey;
  void encrypt_decrypt(char* text, char* result, int key);
  RSAProtocol();
};
