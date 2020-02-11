#include <Arduino.h>
#include "connection.h"
#include "error.h"
#include "udpPacket.h"

class UdpProtocol {
  Connection connection;
  UdpPacket packetRead;
  UdpPacket packetWrite;
  Error error;

  void waitRead();
  void serialFlush();

  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str);
  void formatSendData(char *packet);
  void sendData(char *dataToSend);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);
  void println(char string[10000]);
  void println(int number);

  char *ARDUINO_ERROR;

 public:
  UdpProtocol();

  void printLastError();

  bool arduinoAcceptConnection();
  bool arduinoConnect();
  bool arduinoClose();
  bool arduinoServerClose();

  bool udpWrite(char *dataToSend);

  bool udpRead(char *dataToReceive);
};
