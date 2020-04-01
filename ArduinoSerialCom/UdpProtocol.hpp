#include "ArduinoSerialCom.hpp"
#include "UdpPacket.hpp"

class UdpProtocol : public ArduinoSerialCom {
  UdpPacket packetRead;
  UdpPacket packetWrite;

  void formatSendData(char *packet, int length);
  void sendData(char *dataToSend, int UAID);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive, int &UAID);

 public:
  UdpProtocol();
  bool write(char *dataToSend, int UAID);
  bool read(char *dataToReceive, int &UAID);
};
