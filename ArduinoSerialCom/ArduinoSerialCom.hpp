#include <Arduino.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <math.h>

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

  static int getUniqueArduinoIDFromEEEPROM();

 protected:
  Connection connection;
  Error error;
  HardwareSerial *hardwareSerial;
  SoftwareSerial *softwareSerial;

  void softwareSerial_readBytes(char *data, int length);
  void waitRead();
  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str, int length);
  void computeChecksum(char *data, int &checkSum1, int &checkSum2);
  bool hasPacketErrors(char *data, int _checkSum1, int _checkSum2);

  // char specialChr[2] = "\f";
  char specialChr[2] = ",";

  // arduinoSerialComProtocols protocol;
  void setUniqueArduinoIDToEEPROM(char *UAID);  // unique arduino identifier
};
