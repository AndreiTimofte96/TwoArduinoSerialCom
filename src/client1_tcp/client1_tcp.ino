// UAID = 8808
#include "TcpProtocol.hpp"

TcpProtocol tcpProtocol;

int thisUAID = TcpProtocol::getUniqueArduinoIDFromEEEPROM();

char dataToSend[] = "Salut, eu sunt Andrei si sunt student la Facultatea de Informatica Iasi!";
char dataToReceive[300];
int destinationUAID = 0;
int length;

// long baudRate = 115200;
// long baudRate = 74800;
// long baudRate = 57600;
// long baudRate = 38400;
// long baudRate = 19200;
long baudRate = 9600;

int numberOfTests = 10;
int index = 0;
int okTests = 0;

void setup() {
  tcpProtocol.initializePorts(2, 3);                     // RX, TX
  tcpProtocol.initializeSerial(Serial, baudRate, 1000);  //Serial, baudRate, Serial.setTimeout
  // tcpProtocol.useShowLogs(true);
}

unsigned long start = 0, end = 0, time = 0;

void loop() {
  if (tcpProtocol.connect() < 0) {
    tcpProtocol.printLastError();
  }

  while (index < numberOfTests) {
    index++;
    start = millis();
    // Serial.println(333);
    if ((length = tcpProtocol.write(dataToSend, thisUAID)) < 0) {
      tcpProtocol.printLastError();
    }
    // Serial.println(44444);

    if ((length = tcpProtocol.read(dataToReceive, destinationUAID)) < 0) {
      tcpProtocol.printLastError();
    }
    end = millis();

    time = time + (end - start);
    Serial.println(index);

    if (strcmp(dataToSend, dataToReceive) == 0) {
      okTests++;
    }
  }

  Serial.begin(9600);

  Serial.print("BAUD RATE: ");
  Serial.println(baudRate);

  Serial.print("TIMPUL TOTAL: ");
  Serial.println(time);

  Serial.print("TIMPUL MEDIU: ");
  Serial.println((float)time / (float)numberOfTests);

  Serial.print("NR TEST CORECTE: ");
  Serial.println(okTests);

  Serial.print("NR TOTAL TESTE: ");
  Serial.println(numberOfTests);

  Serial.print("PROCENTAJ: ");
  Serial.println(okTests * 100 / numberOfTests);

  if (tcpProtocol.clientClose() < 0) {
    tcpProtocol.printLastError();
  }  // else

  Serial.println(tcpProtocol.getConnectionStatus());
  while (1)
    ;
}

// Salut, eu sunt Andrei si sunt student la Facultatea de Informatica Iasi!
// Salut, eu sunt Andrei si sunt student la Facultatea de Informatica Iasi si in luna iulie voi prezenta lucrara de licenta
//Salut, eu sunt Andrei si sunt student la Facultatea de Informatica!