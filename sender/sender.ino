// 32,16,8,0,Ana are mere si

////////////DATA STRUCTURES//////////////////
enum blockInformation {
  BLOCK_SIZE = 32,
  BLOCK_HEADER_SIZE = 16,
  BLOCK_BODY_SIZE = 16
};

char CONNECTED_TEXT[] = "CONNECTED";
char DISCONNECTED_TEXT[] = "EROARE_LA_CONECTARE";
char READ_ERROR_TEXT[] = "EROARE_LA_CITIRE";
char WRITE_ERROR_TEXT[] = "EROARE_LA_SCRIERE";
char *ARDUINO_ERROR;

enum connectionStatus {
  DISCONNECTED,
  CONNECTED,
  IDLE,
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

void serialFlush() {
  byte b;
  while (Serial.readBytes(&b, 1) > 0)
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
  Serial.print("EROARE: ");
  Serial.println(getArduinoError());
}
////////////HELPER FUNCTIONS//////////////////

////////////CONNECT AND READ FUNCTIONS///////
bool arduinoAcceptConnection() {
  if (connection.status == DISCONNECTED) {
    Serial.println();
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

bool arduinoClose() {
  serialFlush();
  connection.status = DISCONNECTED;
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
  int remainder = strlen(dataToSend) % myUdpPacket.bLength;
  int bLength = myUdpPacket.bLength;
  int blockLength = bLength;
  for (int blockIndex = 0; blockIndex < myUdpPacket.bNumber; blockIndex++) {
    if (blockIndex == myUdpPacket.bNumber - 1) {
      myUdpPacket.bLength = remainder;
      blockLength = remainder;
    }
    myUdpPacket.bOffset = blockIndex;
    myUdpPacket.bData = (char *)malloc(sizeof(char) * myUdpPacket.bLength + 1);
    memset(myUdpPacket.bData, '\0', sizeof(char) * myUdpPacket.bLength + 1);

    for (int dataIndex = blockIndex * bLength; dataIndex < blockIndex * bLength + blockLength; dataIndex++) {
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
  setArduinoError(WRITE_ERROR_TEXT);
  return false;
}
////////////CONNECT AND READ FUNCTIONS///////

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);

  connection.status = DISCONNECTED;
  myUdpPacket.pSize = BLOCK_SIZE;
  myUdpPacket.bLength = BLOCK_BODY_SIZE;
}

char dataToSend[] = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate. Unu doi trei patru cinci sase sapte opt noua zece.";
char dataToSend1[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book";
char optionToSend[] = "1";

void loop() {
  if (!arduinoAcceptConnection()) {
    printLastError();
  }

  if (!udpWrite(dataToSend1)) {
    printLastError();
  }

  arduinoClose();
}
