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

  int dataSendBits[140];  // 128 bits  + 8 parityBits => 136 bits -> fix 17 bytes
  int dataSendLength = TcpPacket::BLOCK_BODY_SIZE - 1;
  int parityBits = 8;
  int dataSendBitsLength = parityBits + dataSendLength * 8;

  void formatReceiveData(char *bData);
  void receiveData(char *dataToReceive);

  void formatSendConnectionData(char *packet, int packetLength);
  void formatReceiveConnectionData(char *bData);

  void formatSendData(char *packet, int length);
  void sendData(char *dataToSend);

  void encodeWitHammingDistanceCode(char *dataSendString);
  void decodeWitHammingDistanceCode(char *dataSendEncodedString);

  void baseTwoToChar(int *bits, int length, char *str);
  void charToBaseTwo(char *str, int *bits);
  bool isPowerOfTwo(int number);

 public:
  TcpProtocol();
  bool write(char *dataToSend);
  bool read(char *dataToReceive);
  void listen();
  bool connect();
};
