#include <math.h>

#include "ArduinoSerialCom.hpp"
#include "TcpConnectionPacket.hpp"
#include "TcpPacket.hpp"

class TcpProtocol : public ArduinoSerialCom {
  TcpPacket packetRead;
  TcpPacket packetWrite;
  TcpConnectionPacket packetConnectionRead;
  TcpConnectionPacket packetConnectionWrite;
  char **orderedPackets;
  char *dataSendEncodedString;

  bool formatReceiveData(char *bData);
  int receiveData(char *dataToReceive, int &UAID);

  void formatSendConnectionData(char *packet, int packetLength);
  void formatReceiveConnectionData(char *bData);

  void formatSendData(char *packet, int length, int checkSum1, int checkSum2);
  int sendData(char *dataToSend, int UAID);

  void encodeWithHammingDistanceCode(char *dataSendString);
  void decodeWithHammingDistanceCode(char *dataSendEncodedString);

  void baseTwoToChar(int *bits, int length, char *str);
  void charToBaseTwo(char *str, int *bits);
  bool isPowerOfTwo(int number);

 public:
  TcpProtocol();
  int write(char *dataToSend, int UAID);     // UAID = catre cine trimiti
  int read(char *dataToReceive, int &UAID);  // UAID = de la cine primesti
  int listen();
  int connect();
  int clientClose();
  int serverClose();
};
