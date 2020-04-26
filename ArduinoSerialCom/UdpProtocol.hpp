#include "ArduinoSerialCom.hpp"
#include "UdpPacket.hpp"

class UdpProtocol : public ArduinoSerialCom {
  UdpPacket packetRead;
  UdpPacket packetWrite;

  void formatSendData(char *packet, int length);
  int sendData(char *dataToSend, int fromUAID, int toUAID);

  void formatReceiveData(char *bData, char *dataToReceive);
  int receiveData(char *dataToReceive, int &fromUAID, int &toUAID);

 public:
  UdpProtocol();
  int connect();
  int write(char *dataToSend, int toUAID);
  int write(char *dataToSend, int fromUAID, int toUAID);
  int read(char *dataToReceive, int &fromUAID);
  int read(char *dataToReceive, int &fromUAID, int &toUAID);
  int clientClose();
  int serverClose();
};
