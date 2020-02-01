char dataToSend[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";
// 32,16,8,0,Ana are mere si

enum blockInformation {
  BLOCK_SIZE = 32,
  BLOCK_HEADER_SIZE = 16,
  BLOCK_BODY_SIZE = 16
};

char CONNECTED[] = "CONNECTED";
char CONNECTED_ERROR[] = "EROARE_LA_CONECTARE";

struct MyUdpPacket {
  int pSize;
  int bLength;
  int bNumber;
  int bOffset;
  char *bData;
} myUdpPacket;

enum connectionStatus {
  IDLE,
  WAITING,
  READY,
  RUNNING,
};

struct Connection {
  connectionStatus status;
} connection;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);

  connection.status = WAITING;
  myUdpPacket.pSize = 32;
  myUdpPacket.bLength = 16;
}

void addNumberToCharArray(int number, char *str) {
  char aux[100];
  sprintf(aux, "%d,", number);
  strcat(str, aux);
}

void addOffsetToCharArray(char *str) {
  int strLength = strlen(str);
  for (int index = strLength; index < BLOCK_HEADER_SIZE; index++) {
    str[index] = ',';
  }
}

void formatSendData(MyUdpPacket &myUdpPacket, char *packet) {
  addNumberToCharArray(myUdpPacket.pSize, packet);
  addNumberToCharArray(myUdpPacket.bLength, packet);
  addNumberToCharArray(myUdpPacket.bNumber, packet);
  addNumberToCharArray(myUdpPacket.bOffset, packet);
  addOffsetToCharArray(packet);
  strcat(packet, myUdpPacket.bData);
}

void sendData(char *dataToSend) {
  myUdpPacket.bNumber = (strlen(dataToSend) / myUdpPacket.bLength) + 1;

  int bLength = myUdpPacket.bLength;
  for (int blockIndex = 0; blockIndex < myUdpPacket.bNumber; blockIndex++) {
    myUdpPacket.bOffset = blockIndex;
    myUdpPacket.bData = (char *)malloc(sizeof(char) * myUdpPacket.bLength + 1);
    memset(myUdpPacket.bData, '\0', sizeof(char) * myUdpPacket.bLength + 1);

    for (int dataIndex = blockIndex * bLength; dataIndex < blockIndex * bLength + bLength; dataIndex++) {
      int bDataLength = strlen(myUdpPacket.bData);
      myUdpPacket.bData[bDataLength] = dataToSend[dataIndex];
    }

    char packet[myUdpPacket.pSize];
    memset(packet, '\0', sizeof(char) * myUdpPacket.pSize);
    formatSendData(myUdpPacket, packet);
    Serial.write(packet, strlen(packet));
  }
}

void connectArduinoToSlave() {
  Serial.println("WAITING FOR CONNECTION");
  while (!Serial.available())
    ;
  int connectedLength = strlen(CONNECTED);
  char connected[connectedLength];

  Serial.readBytes(connected, connectedLength);

  if (strcmp(connected, CONNECTED) == 0) {
    Serial.write(CONNECTED, connectedLength);
    connection.status = READY;
  } else {
    Serial.write(CONNECTED_ERROR, strlen(CONNECTED_ERROR));
    connection.status = IDLE;
  }
}

void loop() {
  if (connection.status == WAITING) {
    connectArduinoToSlave();
  }
  if (connection.status == READY) {
    connection.status = RUNNING;
    sendData(dataToSend);
    connection.status = READY;
    delay(5000);
  }
}
