// 32,16,8,0,Ana are mere si

char CONNECTED[] = "CONNECTED";
char CONNECTED_ERROR[] = "EROARE_LA_CONECTARE";

enum blockInformation {
  BLOCK_SIZE = 32,
  BLOCK_HEADER_SIZE = 16,
  BLOCK_BODY_SIZE = 16
};

enum connectionStatus {
  IDLE,
  WAITING,
  READY,
  RUNNING,
};

struct Connection {
  connectionStatus status;
} connection;

struct MyUdpPacket {
  int pSize;
  int bLength;
  int bNumber;
  int bOffset;
  char* bData;
} myUdpPacket;

char dataToReceive[1000];

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  Serial.flush();
  delay(10000);
  connection.status = WAITING;
}

void formatReceiveData(MyUdpPacket& myUdpPacket, char* bData) {
  char* pch;
  pch = strtok(bData, ",");
  myUdpPacket.pSize = atoi(pch);

  pch = strtok(NULL, ",");
  myUdpPacket.bLength = atoi(pch);

  pch = strtok(NULL, ",");
  myUdpPacket.bNumber = atoi(pch);

  pch = strtok(NULL, ",");
  myUdpPacket.bOffset = atoi(pch);

  pch = strtok(NULL, ",");
  myUdpPacket.bData = pch;

  strcat(dataToReceive, myUdpPacket.bData);
}

void receiveData(char* dataToReceive) {
  int bDataLength = BLOCK_SIZE;
  char bData[bDataLength];
  memset(bData, '\0', sizeof(char) * bDataLength);
  Serial.readBytes(bData, bDataLength);
  formatReceiveData(myUdpPacket, bData);

  while (myUdpPacket.bOffset + 1 < myUdpPacket.bNumber) {
    while (!Serial.available())
      ;
    memset(bData, '\0', sizeof(char) * bDataLength);
    Serial.readBytes(bData, bDataLength);
    formatReceiveData(myUdpPacket, bData);
  }
}

void connectArduinoToMaster() {
  Serial.write(CONNECTED, strlen(CONNECTED));
  connection.status = READY;

  while (!Serial.available())
    ;
  int connectedLength = strlen(CONNECTED);
  char connected[connectedLength];

  Serial.readBytes(connected, connectedLength);
  Serial.println(connected);

  if (strcmp(connected, CONNECTED) == 0) {
    connection.status = READY;
  } else {
    connection.status = IDLE;
    Serial.println(CONNECTED_ERROR);
  }
}

void loop() {
  if (connection.status == WAITING) {
    connectArduinoToMaster();
  }

  if (connection.status == READY && Serial.available()) {
    connection.status = RUNNING;
    memset(dataToReceive, '\0', sizeof(char) * 1000);
    receiveData(dataToReceive);
    Serial.println("RECEIVED DATA:");
    Serial.println(dataToReceive);
    connection.status = READY;
  }
}
