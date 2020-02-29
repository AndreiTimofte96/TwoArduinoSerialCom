#include <Arduino.h>
#include <SoftwareSerial.h>
#include "BoardType.hpp"
#include "Connection.hpp"
#include "Error.hpp"

class ArduinoSerialCom {
 public:
  ArduinoSerialCom();
  // enum arduinoSerialComProtocols {
  //   UDP,
  //   TCP
  // };

  void initializePorts(int rxPort, int txPort);
  // void initializeProtocol(arduinoSerialComProtocols _protocol);

  void initializeSerial(HardwareSerial &Serial, int beginSpeed, int timeout);
  void initializeSerial(HardwareSerial &Serial, int beginSpeed);

  void printLastError();

  bool arduinoClose();
  bool arduinoServerClose();

  void getBoardType();

 protected:
  Connection connection;
  Error error;
  BoardType boardType;
  HardwareSerial *hardwareSerial;
  SoftwareSerial *softwareSerial;

  void softwareSerial_readBytes(char *data, int length);
  void waitRead();

  // char specialChr[2] = "\f";
  char specialChr[2] = ",";

  // arduinoSerialComProtocols protocol;
};
