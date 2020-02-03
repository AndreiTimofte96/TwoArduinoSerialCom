// 32,16,8,0,Ana are mere si

////////////DATA STRUCTURES//////////////////
enum blockInformation {
  BLOCK_SIZE = 32,
  BLOCK_HEADER_SIZE = 16,
  BLOCK_BODY_SIZE = 16
};

char CONNECTED_TEXT[] = "CONNECTED";
char DISCONNECTED_TEXT[] = "EROARE_LA_CONECTARE";
char* ARDUINO_ERROR;

enum connectionStatus {
  DISCONNECTED,
  CONNECTED,
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
////////////DATA STRUCTURES//////////////////

////////////HELPER FUNCTIONS//////////////////
void waitRead() {
  while (!Serial.available())
    ;
}

void setArduinoError(char* error) {
  ARDUINO_ERROR = (char*)malloc(sizeof(char) * strlen(error) + 1);
  memset(ARDUINO_ERROR, '\0', sizeof(char) * strlen(error) + 1);
  strcpy(ARDUINO_ERROR, error);
}

char* getArduinoError() {
  return ARDUINO_ERROR;
}

void printLastError() {
  Serial.println(getArduinoError());
}
////////////HELPER FUNCTIONS//////////////////

////////////CONNECT AND READ FUNCTIONS///////
bool arduinoConnect() {
  if (connection.status == DISCONNECTED) {
    Serial.write(CONNECTED_TEXT, strlen(CONNECTED_TEXT));
    waitRead();

    int connectedLength = strlen(CONNECTED_TEXT);
    char connected[connectedLength];
    Serial.readBytes(connected, connectedLength);
    Serial.println(connected);

    if (strcmp(connected, CONNECTED_TEXT) == 0) {
      connection.status = CONNECTED;
      return true;
    };
  }

  connection.status = DISCONNECTED;
  setArduinoError(DISCONNECTED_TEXT);
  return false;
}

void formatReceiveData(MyUdpPacket& myUdpPacket, char* bData, char* dataToReceive) {
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

  waitRead();
  memset(bData, '\0', sizeof(char) * bDataLength);
  Serial.readBytes(bData, bDataLength);
  formatReceiveData(myUdpPacket, bData, dataToReceive);

  while (myUdpPacket.bOffset + 1 < myUdpPacket.bNumber) {
    waitRead();
    memset(bData, '\0', sizeof(char) * bDataLength);
    Serial.readBytes(bData, bDataLength);
    formatReceiveData(myUdpPacket, bData, dataToReceive);
  }
}

bool udpRead(char* dataToReceive) {
  if (connection.status == CONNECTED) {
    memset(dataToReceive, '\0', sizeof(char) * sizeof(dataToReceive));
    receiveData(dataToReceive);
    return true;
  }
  setArduinoError(DISCONNECTED_TEXT);
  return false;
}
////////////CONNECT AND READ FUNCTIONS///////

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  Serial.flush();
  delay(10000);
  connection.status = DISCONNECTED;
}

char dataToReceive[1000];
void loop() {
  if (!arduinoConnect()) {
    printLastError();
  }

  if (!udpRead(dataToReceive)) {
    printLastError();
  }
  Serial.println("RECEIVED DATA:");
  Serial.println(dataToReceive);

  //arduinoClose();
}
