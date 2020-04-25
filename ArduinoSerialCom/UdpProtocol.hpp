#include "ArduinoSerialCom.hpp"
#include "UdpPacket.hpp"

class UdpProtocol : public ArduinoSerialCom {
  UdpPacket packetRead;
  UdpPacket packetWrite;

  void formatSendData(char *packet, int length);
  void sendData(char *dataToSend, int fromUAID, int toUAID);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive, int &fromUAID, int &toUAID);

 public:
  UdpProtocol();
  bool write(char *dataToSend, int toUAID);
  bool write(char *dataToSend, int fromUAID, int toUAID);
  bool read(char *dataToReceive, int &fromUAID);
  bool read(char *dataToReceive, int &fromUAID, int &toUAID);
  void clientClose();
  void serverClose();
};
