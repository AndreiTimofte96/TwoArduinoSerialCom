#include "ArduinoSerialCom.hpp"
#include "UdpPacket.hpp"

class UdpProtocol : public ArduinoSerialCom {
  UdpPacket packetRead;
  UdpPacket packetWrite;

  void formatSendData(char *packet, int length);
  void sendData(char *dataToSend);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);

 public:
  UdpProtocol();
  bool write(char *dataToSend);
  bool read(char *dataToReceive);
};
