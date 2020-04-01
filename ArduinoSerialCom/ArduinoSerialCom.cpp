#include "ArduinoSerialCom.hpp"

ArduinoSerialCom::ArduinoSerialCom() {
}

void ArduinoSerialCom::initializePorts(int rxPort, int txPort) {
  softwareSerial = new SoftwareSerial(rxPort, txPort);
  softwareSerial->begin(9600);
}

// void ArduinoSerialCom::initializeProtocol(arduinoSerialComProtocols _protocol) {
//   protocol = _protocol;
// }

void ArduinoSerialCom::initializeSerial(HardwareSerial &Serial, int beginSpeed, int timeout) {
  hardwareSerial = &Serial;
  hardwareSerial->begin(beginSpeed);
  hardwareSerial->setTimeout(timeout);
}

void ArduinoSerialCom::initializeSerial(HardwareSerial &Serial, int beginSpeed) {
  initializeSerial(Serial, beginSpeed, 1000);
}

void ArduinoSerialCom::printLastError() {
  hardwareSerial->print("EROARE: ");
  hardwareSerial->println(error.getError());
}

bool ArduinoSerialCom::arduinoServerClose() {
  while (1)
    ;
  connection.setStatus(Connection::IDLE);
}

bool ArduinoSerialCom::arduinoClose() {
  while (1)
    ;
  connection.setStatus(Connection::IDLE);
}

void ArduinoSerialCom::getBoardType() {
  hardwareSerial->print("Detected board: ");
  hardwareSerial->println(BOARD);
}

void ArduinoSerialCom::waitRead() {
  while (!softwareSerial->available())
    ;
}

void ArduinoSerialCom::softwareSerial_readBytes(char *data, int length) {
  int index;
  // String str;
  for (index = 0; index < length; index++) {
    char c = softwareSerial->read();
    delay(1);

    if ((int)c == -1) break;

    data[index] = c;
  }
  data[index] = '\0';
}

void ArduinoSerialCom::addNumberToCharArray(int number, char *str) {
  char aux[10];
  sprintf(aux, "%d", number);
  strcat(str, aux);
  strcat(str, specialChr);
}

void ArduinoSerialCom::addOffsetToCharArray(char *str, int length) {
  int strLength = strlen(str);
  for (int index = strLength; index < length; index++) {
    str[index] = specialChr[0];
  }
}

void ArduinoSerialCom::computeChecksum(char *data, int &checkSum1, int &checkSum2) {
  for (int index = 0; index < strlen(data); index++) {
    checkSum1 = (checkSum1 + data[index]) % 255;
    checkSum2 += checkSum1 % 255;
  }
  checkSum1 %= 255;
  checkSum2 %= 255;
}

bool ArduinoSerialCom::hasPacketErrors(char *data, int _checkSum1, int _checkSum2) {
  int checkSum1 = 0, checkSum2 = 0;
  computeChecksum(data, checkSum1, checkSum2);
  if (checkSum1 == _checkSum1 && checkSum2 == _checkSum2) {
    return false;
  }
  return true;
}

void ArduinoSerialCom::setUniqueArduinoIDToEEPROM(char *UAID) {
  byte length = strlen(UAID);
  int addrOffset = 0;
  EEPROM.write(addrOffset, length);
  for (int index = 0; index < length; index++) {
    EEPROM.write(addrOffset + 1 + index, UAID[index]);
  }
}

int ArduinoSerialCom::getUniqueArduinoIDFromEEEPROM() {
  int addrOffset = 0;
  int length = EEPROM.read(addrOffset);
  int UAIDLength = 4;  // modify UAID length;
  char *UAID;
  UAID = (char *)malloc(sizeof(char) * UAIDLength);

  if (length == 255 || length != UAIDLength) {
    randomSeed(analogRead(0));
    long randId = random(1000, 9999);
    sprintf(UAID, "%d", randId);
    setUniqueArduinoIDToEEPROM(UAID);
    delay(10);
    return atoi(UAID);
  }
  int index;
  for (index = 0; index < UAIDLength; index++) {
    UAID[index] = EEPROM.read(addrOffset + 1 + index);
  }
  UAID[index] = '\0';

  return atoi(UAID);
}