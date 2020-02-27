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
