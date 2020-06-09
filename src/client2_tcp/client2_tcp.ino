// UAID = 4987
#include <TcpProtocol.hpp>

TcpProtocol tcpProtocol;

int thisUAID = TcpProtocol::getUniqueArduinoIDFromEEEPROM();

char dataToReceive[300];
int destinationUAID = 0;  //should be 8808;
int length;

String str;
int strLength;

void setup() {
  tcpProtocol.initializePorts(2, 3);                       // RX, TX
  tcpProtocol.initializeSerial(Serial, 9600, 500, false);  //Serial, baudRate, Serial.setTimeout
}

float start = 0, end = 0;

void loop() {
  if ((destinationUAID = tcpProtocol.listen()) < 0) {
    tcpProtocol.printLastError();
  }

  // int ok = 2;
  // while (ok) {
  //   ok--;

  //   if ((length = tcpProtocol.read(dataToReceive, destinationUAID)) < 0) {
  //     tcpProtocol.printLastError();
  //   }

  //   Serial.print("\nRECEIVED DATA THROUGH SERVER FROM CLIENT ");
  //   Serial.print(destinationUAID);
  //   Serial.println(":");
  //   Serial.println(dataToReceive);
  //   Serial.println(length);
  //   Serial.println();

  //   while (!Serial.available())
  //     ;
  //   str = Serial.readStringUntil('\n');
  //   strLength = str.length();
  //   char userInput[strLength + 1];
  //   str.toCharArray(userInput, strLength + 1);

  //   Serial.println(userInput);

  //   if ((length = tcpProtocol.write(userInput, thisUAID)) < 0) {
  //     tcpProtocol.printLastError();
  //   }
  //   Serial.println("WRITE LENGTH");
  //   Serial.println(length);
  // }
  while (1) {
    if ((length = tcpProtocol.read(dataToReceive, destinationUAID)) < 0) {
      tcpProtocol.printLastError();
    }

    if ((length = tcpProtocol.write(dataToReceive, thisUAID)) < 0) {
      tcpProtocol.printLastError();
    }

    // Serial.print("\nRECEIVED DATA THROUGH SERVER FROM CLIENT ");
    // Serial.print(destinationUAID);
    // Serial.println(":");
    // Serial.println(dataToReceive);
    // Serial.println(length);
    // Serial.println(end - start);
  }

  if (tcpProtocol.serverClose() < 0) {
    tcpProtocol.printLastError();
  }

  while (1)
    ;
}