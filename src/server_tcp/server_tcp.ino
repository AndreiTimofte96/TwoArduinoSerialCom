#include <TcpProtocol.hpp>
// 32,16,8,0,Ana are mere si

TcpProtocol tcpProtocol1, tcpProtocol2;

void setup() {
  tcpProtocol1.initializePorts(2, 3);                // RX, TX
  tcpProtocol1.initializeSerial(Serial, 9600, 500);  //Serial, baudRate, Serial.setTimeout

  tcpProtocol2.initializePorts(8, 9);  // RX, TX
  tcpProtocol2.initializeSerial(Serial, 9600, 500);
}

char dataToReceive[300];
int UAIDList[2];
int UAIDListLength = 0;

void loop() {
  UAIDList[UAIDListLength++] = tcpProtocol1.listen();
  UAIDList[UAIDListLength++] = tcpProtocol2.listen();

  Serial.println(UAIDList[0]);
  Serial.println(UAIDList[1]);

  while (1) {
    if (!tcpProtocol1.read(dataToReceive)) {
      tcpProtocol1.printLastError();
    }
    Serial.println("\nRECEIVED DATA FROM CLIENT 1:");
    Serial.println(dataToReceive);

    if (!tcpProtocol2.write(dataToReceive)) {
      tcpProtocol2.printLastError();
    }

    if (!tcpProtocol2.read(dataToReceive)) {
      tcpProtocol2.printLastError();
    }
    Serial.println("\nRECEIVED DATA FROM CLIENT 2:");
    Serial.println(dataToReceive);

    if (!tcpProtocol1.write(dataToReceive)) {
      tcpProtocol1.printLastError();
    }
  }

  tcpProtocol1.arduinoServerClose();
  tcpProtocol2.arduinoServerClose();
}

// char dataToSendOption1[] = "Ana are mere si gutui si tata are pere";
// char dataToSendOption1[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";

// char dataToSendOption2[] = "Lorem Ipsum is simply dummy text of the ";
// char dataToSendOption2[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industrys standard dummy text ever since the 1500s, when an unknown printer took a galley!";
// char dataToSendOption3[] = "Contrary to popular belief, Lorem Ipsum is not simply random text. It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of 'de Finibus Bonorum et Malorum' (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of ethics, very popular during the Renaissance. The first line of Lorem Ipsum, 'Lorem ipsum dolor sit amet..', comes from a line in section 1.10.32. The standard chunk of Lorem Ipsum used since the 1500s is reproduced below for those interested. Sections 1.10.32 and 1.10.33 from 'de Finibus Bonorum et Malorum' by Cicero are also reproduced in their exact original form.";