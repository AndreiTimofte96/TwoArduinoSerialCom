#include <TcpProtocol.hpp>
// 32,16,8,0,Ana are mere si

TcpProtocol tcpProtocol;

void setup() {
  tcpProtocol.initializePorts(2, 3);                // RX, TX
  tcpProtocol.initializeSerial(Serial, 9600, 500);  //Serial, baudRate, Serial.setTimeout
}

char dataToSendOption1[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";
char dataToSendOption2[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industrys standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book";
// char dataToSendOption2[] = "Contrary to popular belief, Lorem Ipsum is not simply random text. It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of 'de Finibus Bonorum et Malorum' (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of ethics, very popular during the Renaissance. The first line of Lorem Ipsum, 'Lorem ipsum dolor sit amet..', comes from a line in section 1.10.32. The standard chunk of Lorem Ipsum used since the 1500s is reproduced below for those interested. Sections 1.10.32 and 1.10.33 from 'de Finibus Bonorum et Malorum' by Cicero are also reproduced in their exact original form.";

char option[10];

void loop() {
  // tcpProtocol.getBoardType();

  tcpProtocol.listen();

  if (!tcpProtocol.read(option)) {
    tcpProtocol.printLastError();
  }
  // Serial.println("option:");
  // Serial.println(option);

  if (strcmp(option, "2") == 0) {
    if (!tcpProtocol.write(dataToSendOption2)) {
      tcpProtocol.printLastError();
    }
  } else {
    if (!tcpProtocol.write(dataToSendOption1)) {
      tcpProtocol.printLastError();
    }
  }

  tcpProtocol.arduinoServerClose();
}
