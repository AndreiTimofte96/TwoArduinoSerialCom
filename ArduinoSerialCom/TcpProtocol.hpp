#include "ArduinoSerialCom.hpp"
#include "TcpConnection.hpp"
#include "TcpPacket.hpp"

class TcpProtocol : public ArduinoSerialCom {
  TcpPacket packetRead;
  TcpPacket packetWrite;
  TcpConnection packetConnectionRead;
  TcpConnection packetConnectionWrite;
  char **orderedPackets;

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);

  void formatSendConnectionData(char *packet, int packetLength);
  void formatReceiveConnectionData(char *bData);

  void formatSendData(char *packet, int length);
  void sendData(char *dataToSend);

 public:
  TcpProtocol();
  bool write(char *dataToSend);
  bool read(char *dataToReceive);
  void listen();
  bool connect();
};
