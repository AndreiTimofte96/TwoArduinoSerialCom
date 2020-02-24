#include <Arduino.h>
#include <SoftwareSerial.h>
#include "boardType.h"
#include "connection.h"
#include "error.h"
#include "udpPacket.h"

class UdpProtocol {
  Connection connection;
  UdpPacket packetRead;
  UdpPacket packetWrite;
  Error error;
  HardwareSerial *_Serial;
  BoardType boardType;
  SoftwareSerial *softwareSerial;

  void softwareSerial_readBytes(char *data, int length);
  void waitRead();
  void computeChecksum(char *data, int &checkSum1, int &checkSum2);
  bool hasPacketErrors(char *data);

  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str);
  void formatSendData(char *packet);
  void sendData(char *dataToSend);

  void formatReceiveData(char *bData, char *dataToReceive);
  void receiveData(char *dataToReceive);

  char *ARDUINO_ERROR;
  char specialChr[2] = "\f";

 public:
  UdpProtocol(int rxPort, int txPort);
  void initPort(HardwareSerial &Serial, int beginSpeed, int timeout);
  void initPort(HardwareSerial &Serial, int beginSpeed);

  void printLastError();

  bool arduinoClose();
  bool arduinoServerClose();

  bool udpWrite(char *dataToSend);

  bool udpRead(char *dataToReceive);

  void getBoardType();
};
