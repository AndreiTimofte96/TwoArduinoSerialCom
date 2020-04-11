#include <AESLib.h>
#include <Arduino.h>

class TLSProtocol {
  bool useTLSProtocol;

 public:
  int primeProd, publicKey, privateKey;
  char *masterKey;
  bool useMasterKey;

  void setUseTLSProtocol(bool use);
  bool getUseTLSProtocol();

  void setUseMasterKey(bool _useMasterKey);
  bool getUseMasterKey();

  TLSProtocol();

  void generateRandomString(char *str);
  void generateMasterKey(char *preMasterKey, char *randomFctString, char *masterKey);

  void encryptAES_CBC(char *data, char *result);
  void decryptAES_CBC(char *data, char *result);
  void encryptRSA_decryptRSA(char *text, int key, char *result);
};
