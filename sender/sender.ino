
void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
  Serial.setTimeout(500);
}

String data = "Ana are mere si gutui si tata are pere si eu vreau sa le mananc pe toate.";

bool sendData(String data) {
  int blockSize = 8;
  char block[blockSize];

  for (int index = 0; index < data.length(); index++) {
    for (int blockIndex = 0; blockIndex < blockSize; blockIndex++) {
      block[blockIndex] = data[index++];
    }
    index--;
    int iBlockLength = sizeof(block);
    char sendData[100];
    snprintf(sendData, 100, "%d%s", iBlockLength, block);
    int iSendDataLength = strlen(sendData);

    Serial.write(sendData, iSendDataLength);
    block[0] = '\0';
  }
}

void loop() {
  sendData(data);
  delay(5000);
}
