#include <Arduino.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

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

  void useShowLogs(bool _SHOW_LOGS);
  void useAsyncMode(bool _ASYNC_MODE);

  void printLastError();
  const char *getConnectionStatus();

  static int getUniqueArduinoIDFromEEEPROM();

 protected:
  Connection connection;
  Error error;
  HardwareSerial *hardwareSerial;
  SoftwareSerial *softwareSerial;
  bool SHOW_LOGS = false;
  bool ASYNC_MODE = false;

  void softwareSerial_readBytes(char *data, int length);
  void waitRead();
  void addNumberToCharArray(int number, char *str);
  void addOffsetToCharArray(char *str, int length);
  void computeChecksum(char *data, int &checkSum1, int &checkSum2);
  bool hasPacketErrors(char *data, int _checkSum1, int _checkSum2);
  void whileForever();

  void printLog(char *log);
  void printLog(const __FlashStringHelper *log);
  void printLog(int log);
  void printlnLog(char *log);
  void printlnLog(const __FlashStringHelper *log);
  void printlnLog(int log);

  int checkConnectionStatus(Error::errorMessages errorMessage);

  // char specialChr[2] = "\f";
  char specialChr[2] = ",";

  // arduinoSerialComProtocols protocol;
  static void setUniqueArduinoIDToEEPROM(char *UAID);  // unique arduino identifier
};
