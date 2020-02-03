// 32,16,8,0,Ana are mere si

////////////DATA STRUCTURES//////////////////
enum blockInformation {
  BLOCK_SIZE = 32,
  BLOCK_HEADER_SIZE = 16,
  BLOCK_BODY_SIZE = 16
};

char CONNECTED_TEXT[] = "CONNECTED";
char DISCONNECTED_TEXT[] = "EROARE_LA_CONECTARE";
char *ARDUINO_ERROR;

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
  char *bData;
} myUdpPacket;
////////////DATA STRUCTURES//////////////////

////////////HELPER FUNCTIONS//////////////////
void waitRead() {
  while (!Serial.available())
    ;
}

void setArduinoError(char *error) {
  ARDUINO_ERROR = (char *)malloc(sizeof(char) * strlen(error) + 1);
  memset(ARDUINO_ERROR, '\0', sizeof(char) * strlen(error) + 1);
  strcpy(ARDUINO_ERROR, error);
}

char *getArduinoError() {
  return ARDUINO_ERROR;
}

void printLastError() {
  Serial.println(getArduinoError());
}
////////////HELPER FUNCTIONS//////////////////

////////////CONNECT AND READ FUNCTIONS///////
bool arduinoAcceptConnection() {
  if (connection.status == DISCONNECTED) {
    Serial.println("WAITING FOR CONNECTION");

    int connectedLength = strlen(CONNECTED_TEXT);
    char connected[connectedLength];

    waitRead();
    Serial.readBytes(connected, connectedLength);

    if (strcmp(connected, CONNECTED_TEXT) == 0) {
      Serial.write(CONNECTED_TEXT, connectedLength);
      connection.status = CONNECTED;
      return true;
    }

    Serial.write(DISCONNECTED_TEXT, strlen(DISCONNECTED_TEXT));
    connection.status = DISCONNECTED;
    setArduinoError(DISCONNECTED_TEXT);
    return false;
  }

  connection.status = DISCONNECTED;
  setArduinoError(DISCONNECTED_TEXT);
  return false;
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

bool udpWrite(char *dataToSend) {
  if (connection.status == CONNECTED) {
    sendData(dataToSend);
    return true;
  }
  setArduinoError(DISCONNECTED_TEXT);
  return false;
}
////////////CONNECT AND READ FUNCTIONS///////

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);

  connection.status = DISCONNECTED;
  myUdpPacket.pSize = 32;
  myUdpPacket.bLength = 16;
}

char dataToSend[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";
void loop() {
  if (!arduinoAcceptConnection()) {
    printLastError();
  }

  if (!udpWrite(dataToSend)) {
    printLastError();
  }
  delay(5000);
  //arduinoClose();
}
