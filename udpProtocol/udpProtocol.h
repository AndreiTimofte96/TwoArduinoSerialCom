#include <Arduino.h>
#include "connection.h"
#include "udpPacket.h"

class UdpProtocol {
  Connection connection;
  UdpPacket packetRead;
  UdpPacket packetWrite;

  void waitRead();
  void serialFlush();

  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str);
  void formatSendData(char *packet);
  void sendData(char *dataToSend);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);

  char *ARDUINO_ERROR;

 public:
  UdpProtocol();

  char *getArduinoError();
  void setArduinoError(char *error);
  void printLastError();

  bool arduinoAcceptConnection();
  bool arduinoConnect();
  bool arduinoClose();
  bool arduinoServerClose();

  bool udpWrite(char *dataToSend);

  bool udpRead(char *dataToReceive);
};
