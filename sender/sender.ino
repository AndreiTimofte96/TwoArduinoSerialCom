char dataToSend[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";
// 32,16,8,0,Ana are mere si

struct MyUdpPacket {
  int pSize;
  int bLength;
  int bNumber;
  int bOffset;
  char *bData;
} myUdpPacket;

enum connectionStatus {
  READY,
  RUNNING,
};

struct Connection {
  connectionStatus status;
} connection;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);

  connection.status = READY;
  myUdpPacket.pSize = 32;
  myUdpPacket.bLength = 16;
  delay(10000);
}

void addNumberToCharArray(int number, char *str) {
  char aux[100];
  sprintf(aux, "%d,", number);
  strcat(str, aux);
}
void formatSendData(MyUdpPacket &myUdpPacket, char *packet) {
  addNumberToCharArray(myUdpPacket.pSize, packet);
  addNumberToCharArray(myUdpPacket.bLength, packet);
  addNumberToCharArray(myUdpPacket.bNumber, packet);
  addNumberToCharArray(myUdpPacket.bOffset, packet);
  strcat(packet, myUdpPacket.bData);
}

bool sendData(char *dataToSend) {
  myUdpPacket.bNumber = (strlen(dataToSend) / myUdpPacket.bLength) + 1;

  int bLength = myUdpPacket.bLength;
  for (int blockIndex = 0; blockIndex < myUdpPacket.bNumber; blockIndex++) {
    myUdpPacket.bOffset = blockIndex;
    myUdpPacket.bData = (char *)malloc(sizeof(char) * myUdpPacket.bLength + 1);
    memset(myUdpPacket.bData, '\0', sizeof(char) * myUdpPacket.bLength + 1);

    for (int dataIndex = blockIndex * bLength; dataIndex < blockIndex * bLength + bLength; dataIndex++) {
      int bDataLength = strlen(myUdpPacket.bData);
      myUdpPacket.bData[bDataLength] = dataToSend[dataIndex];
    }  // formam blocurile de cate 12
    // myUdpPacket.bData[strlen(myUdpPacket.bData) - 1] = '\0';

    char packet[myUdpPacket.pSize];
    memset(packet, '\0', sizeof(char) * myUdpPacket.pSize);
    formatSendData(myUdpPacket, packet);
    Serial.write(packet, strlen(packet));
    delay(1000);
  }
}

void loop() {
  if (connection.status == READY) {
    connection.status = RUNNING;
    sendData(dataToSend);
    connection.status = READY;
  }
  delay(5000);
}
