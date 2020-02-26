#include "ArduinoSerialCom.hpp"
#include "TcpPacket.hpp"

class TcpProtocol : public ArduinoSerialCom {
  TcpPacket packetRead;
  TcpPacket packetWrite;

  // void computeChecksum(char *data, int &checkSum1, int &checkSum2);
  // bool hasPacketErrors(char *data);

  // void addNumberToCharArray(int number, char *str);
  // void addOffsetToCharArray(char *str);
  // void formatSendData(char *packet);
  // void sendData(char *dataToSend);

  // void formatReceiveData(char *bData, char *dataToReceive);
  // void receiveData(char *dataToReceive);

 public:
  TcpProtocol();
  bool udpWrite(char *dataToSend);
  bool udpRead(char *dataToReceive);
};
