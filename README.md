# TwoArduinoSerialCom

TwoArduinoSerialCom  also named two-arduino-COM, is an Arduino Library to transfer serial data, ensuring data integrity, via Arduino Serial Communication UART device.

This library supports two modes of communication, following the client - server model: without confirmation, similar to the UDP protocol, and with confirmation  that the message has been received successfully, similar to the TCP protocol.

**Library features:**

• interconnection mechanism between two Arduinos based on a UAID;

• availability of all ports for message communication which allows connecting multiple Arduino boards within the same sketch;

• creates a unique id for each Arduino board and saves it into EEPROM memory;

• sending messages in two ways: with and without confirmation;

• message segmentation into fixed-size blocks;

• detection and correction of messages at the destination in case of errors in the transmission process;

• mechanism for reordering the received packets;

• connection termination mechanism;




In both ways, the library initialization contains common elements:

1. Include the library and create UdpProtocol / TcpProcol class instance:
```c++
#include “UdpProtocol.hpp” // without confirmation
UdpProtocol myProtocol;
or
#include “TcpProtocol.hpp” //  with confirmation
TcpProtocol myProtocol;
```
2. Initialize the library

```c++
myProtocol.initializePorts(rxPort, txPort);

myProtocol.initializeSerial(Serial, 9600, 500); //Serial port, BAUD rate, Serial timeout
```


# 1. *Communication without confirmation*

```c++

if ((length = udpProtocol.write(text, UAID)) < 0) { // text, Arduino board destination UAID
      udpProtocol.printLastError(); // prints last error
    }

if (!udpProtocol.read(dataToReceive, fromUAID)) { // text, Ardunio board sender UAID
      udpProtocol.printLastError();
}
```
# 2. *Communication with confirmation*

First we need to interconnect the boards:

Server Side: 
```c++
  if ((clientUAID = tcpProtocol1.listen()) < 0) {
    tcpProtocol1.printLastError();
  }
```

Client Side: 
```c++
 if (tcpProtocol.connect() < 0) {
    tcpProtocol.printLastError();
  }
```

Then we can start serial transfer:
```c++
if ((length = tcpProtocol.write(text, destinationUAID)) < 0) {
    tcpProtocol.printLastError();
}
   

if ((length = tcpProtocol.read(dataToReceive, senderUAID)) < 0) {
    tcpProtocol.printLastError();
}
```

Closing connection server-side:
```c++
if (tcpProtocol.serverClose() < 0) {
    tcpProtocol.printLastError();
  }  // else
  while (1);
```

Closing connection client-side:
```c++
if (tcpProtocol.clientClose() < 0) {
    tcpProtocol.printLastError();
  }  // else
  while (1);
```

# **Complete Client side Code with TcpProtocol class**:
```c++
// UAID = 8808
#include "TcpProtocol.hpp"

TcpProtocol myProtocol;

void setup() {
  myProtocol.initializePorts(2, 3);                       // RX, TX
  myProtocol.initializeSerial(Serial, 9600, 500, false);  //Serial, baudRate, Serial.setTimeout
}

char dataToSend[] = "Some text sent by client";
char dataToReceive[300];

int destinationUAID = 4987;
int senderUAID;
int length;

void loop() {
  if (myProtocol.connect() < 0) {
    myProtocol.printLastError();
  }

  if ((length = myProtocol.write(dataToSend, destinationUAID)) < 0) {
    myProtocol.printLastError();
  }

  if ((length = myProtocol.read(dataToReceive, senderUAID)) < 0) {
    myProtocol.printLastError();
  }

  Serial.print(senderUAID);
  Serial.println(":");
  Serial.println(dataToReceive);
  Serial.println(length);

  if (myProtocol.clientClose() < 0) {
    myProtocol.printLastError();
  }  // else
  while (1)
    ;
}
```

# **Complete Server side Code with TcpProtocol class**:
```c++
// UAID = 4987
#include <TcpProtocol.hpp>

TcpProtocol myProtocol;

int serverUAID = TcpProtocol::getUniqueArduinoIDFromEEEPROM();

char dataToSend[] = "Some text sent by server";
char dataToReceive[300];

int destinationUAID;  //should be 8808;
int senderUAID;
int length;

void setup() {
  myProtocol.initializePorts(2, 3);                       // RX, TX
  myProtocol.initializeSerial(Serial, 9600, 500, false);  //Serial, baudRate, Serial.setTimeout
}

void loop() {
  if ((destinationUAID = myProtocol.listen()) < 0) {
    myProtocol.printLastError();
  }

  if ((length = myProtocol.read(dataToReceive, senderUAID)) < 0) {
    myProtocol.printLastError();
  }
  Serial.print(senderUAID);
  Serial.println(":");
  Serial.println(dataToReceive);
  Serial.println(length);

  if ((length = myProtocol.write(dataToSend, destinationUAID)) < 0) {
    myProtocol.printLastError();
  }

  if (myProtocol.serverClose() < 0) {
    myProtocol.printLastError();
  }

  while (1)
    ;
}
```
