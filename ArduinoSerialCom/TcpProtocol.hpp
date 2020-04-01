#include <math.h>

#include "ArduinoSerialCom.hpp"
#include "TcpConnection.hpp"
#include "TcpPacket.hpp"

class TcpProtocol : public ArduinoSerialCom {
  TcpPacket packetRead;
  TcpPacket packetWrite;
  TcpConnection packetConnectionRead;
  TcpConnection packetConnectionWrite;
  char **orderedPackets;
  char *dataSendEncodedString;

  bool formatReceiveData(char *bData);
  void receiveData(char *dataToReceive, int &UAID);

  void formatSendConnectionData(char *packet, int packetLength);
  void formatReceiveConnectionData(char *bData);

  void formatSendData(char *packet, int length, int checkSum1, int checkSum2);
  void sendData(char *dataToSend, int UAID);

  void encodeWitHammingDistanceCode(char *dataSendString);
  void decodeWitHammingDistanceCode(char *dataSendEncodedString);

  void baseTwoToChar(int *bits, int length, char *str);
  void charToBaseTwo(char *str, int *bits);
  bool isPowerOfTwo(int number);

 public:
  TcpProtocol();
  bool write(char *dataToSend, int UAID);     // UAID = catre cine trimiti
  bool read(char *dataToReceive, int &UAID);  // UAID = de la cine primesti
  int listen();
  bool connect();
};
