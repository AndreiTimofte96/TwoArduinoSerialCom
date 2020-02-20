#include <Arduino.h>
#include "connection.h"
#include "error.h"
#include "udpPacket.h"

class UdpProtocol {
  Connection connection;
  UdpPacket packetRead;
  UdpPacket packetWrite;
  Error error;
  HardwareSerial *_Serial;

  void waitRead();
  void serialFlush();
  void computeChecksum(char *data, int &checkSum1, int &checkSum2);
  bool hasPacketErrors(char *data);

  void println(char string[10000]);
  void println(int number);

  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str);
  void formatSendData(char *packet);
  void sendData(char *dataToSend);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);

  char *ARDUINO_ERROR;
  char specialChr[2];

 public:
  UdpProtocol();
  void initPort(HardwareSerial &Serial, int beginSpeed, int timeout);

  void printLastError();

  bool arduinoAcceptConnection();
  bool arduinoConnect();
  bool arduinoClose();
  bool arduinoServerClose();

  bool udpWrite(char *dataToSend);

  bool udpRead(char *dataToReceive);
};
