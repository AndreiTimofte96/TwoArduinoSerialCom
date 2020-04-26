// UAID = 3607
#include <UdpProtocol.hpp>

UdpProtocol udpProtocol1, udpProtocol2;

struct {
  int rxPort;
  int txPort;
  int UAID;
} clients[2];
int clientsLength = 2;
int serverUAID = UdpProtocol::getUniqueArduinoIDFromEEEPROM();

char dataToReceive[300];
int fromUAID;
int toUAID;
char clientUAIDError[] = "Invalid client UAID";

void clientsSetup() {
  clients[0].rxPort = 2;
  clients[0].txPort = 3;

  clients[1].rxPort = 8;
  clients[1].txPort = 9;
}

void setup() {
  clientsSetup();

  udpProtocol1.initializePorts(clients[0].rxPort, clients[0].txPort);  // RX, TX
  udpProtocol1.initializeSerial(Serial, 9600, 500, false);             //Serial, baudRate, Serial.setTimeout

  udpProtocol2.initializePorts(clients[1].rxPort, clients[1].txPort);  // RX, TX
  udpProtocol2.initializeSerial(Serial, 9600, 500, false);
}

void loop() {
  if (udpProtocol1.connect() < 0) {
    udpProtocol1.printLastError();
  }
  if (udpProtocol2.connect() < 0) {
    udpProtocol2.printLastError();
  }

  int ok = 2;
  int length;
  while (ok) {
    ok--;

    if ((length = udpProtocol1.read(dataToReceive, fromUAID, toUAID)) < 0) {
      udpProtocol1.printLastError();
    }
    Serial.println("\nRECEIVED DATA FROM CLIENT 1:");
    Serial.println(dataToReceive);
    Serial.println(fromUAID);

    if ((length = udpProtocol2.write(dataToReceive, fromUAID, toUAID)) < 0) {
      udpProtocol2.printLastError();
    }

    if ((length = udpProtocol2.read(dataToReceive, fromUAID, toUAID)) < 0) {
      udpProtocol2.printLastError();
    }
    Serial.println("\nRECEIVED DATA FROM CLIENT 2:");
    Serial.println(fromUAID);

    if ((length = udpProtocol1.write(dataToReceive, fromUAID, toUAID)) < 0) {
      udpProtocol1.printLastError();
    }
  }

  if (udpProtocol1.serverClose() < 0) {
    udpProtocol1.printLastError();
  }
  if (udpProtocol2.serverClose() < 0) {
    udpProtocol2.printLastError();
  }
  while (1)
    ;
}

// char dataToSendOption1[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";

// char dataToSendOption2[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industrys standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book";
// char dataToSendOption3[] = "Contrary to popular belief, Lorem Ipsum is not simply random text. It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of 'de Finibus Bonorum et Malorum' (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of ethics, very popular during the Renaissance. The first line of Lorem Ipsum, 'Lorem ipsum dolor sit amet..', comes from a line in section 1.10.32. The standard chunk of Lorem Ipsum used since the 1500s is reproduced below for those interested. Sections 1.10.32 and 1.10.33 from 'de Finibus Bonorum et Malorum' by Cicero are also reproduced in their exact original form."