#include "ArduinoSerialCom.hpp"
#include "UdpPacket.hpp"

class UdpProtocol : public ArduinoSerialCom {
  UdpPacket packetRead;
  UdpPacket packetWrite;

  void computeChecksum(char *data, int &checkSum1, int &checkSum2);
  bool hasPacketErrors(char *data);

  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str);
  void formatSendData(char *packet);
  void sendData(char *dataToSend);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);

 public:
  UdpProtocol();
  bool write(char *dataToSend);
  bool read(char *dataToReceive);
};
