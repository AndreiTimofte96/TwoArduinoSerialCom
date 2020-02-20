#include "udpProtocol.h"
// 32,16,8,0,Ana are mere si

UdpProtocol udpProtocol;

void setup() {
  //   Serial.begin(9600);
  //   Serial.setTimeout(500);
  udpProtocol.initPort(Serial, 9600, 500);
}

char dataToSend1[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";
char dataToSend2[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industrys standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book";

char option[10];

char dataToSend[250];

void loop() {
  //   if (!udpProtocol.arduinoAcceptConnection()) {
  //     udpProtocol.printLastError();
  //   }
  if (!udpProtocol.udpRead(option)) {
    udpProtocol.printLastError();
  }

  if (strcmp(option, "1") == 0) {
    strcpy(dataToSend, dataToSend1);
  }

  if (strcmp(option, "2") == 0) {
    strcpy(dataToSend, dataToSend2);
  }

  if (!udpProtocol.udpWrite(dataToSend)) {
    udpProtocol.printLastError();
  }

  udpProtocol.arduinoServerClose();
}
