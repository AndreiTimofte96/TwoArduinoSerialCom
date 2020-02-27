#include "ArduinoSerialCom.hpp"
#include "TcpPacket.hpp"

class TcpProtocol : public ArduinoSerialCom {
  TcpPacket packetRead;
  TcpPacket packetWrite;

  // void computeChecksum(char *data, int &checkSum1, int &checkSum2);
  // bool hasPacketErrors(char *data);

  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);

  void establishConnectionServer();
  bool establishConnectionClient();

  void formatSendData(char *packet);
  void sendData(char *dataToSend);

 public:
  TcpProtocol();
  bool write(char *dataToSend);
  bool read(char *dataToReceive);
  void listen();
  bool connect();
};
