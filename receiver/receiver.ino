
void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
  Serial.setTimeout(500);
}

char finalResult[1000];
void loop() {
  if (Serial.available()) {
    int dataLength = Serial.parseInt();
    Serial.print("DATA LENGTH: ");
    Serial.println(dataLength);
    char data[dataLength];

    Serial.readBytes(data, dataLength);
    Serial.print("RECEIVED: ");
    // for (int index = 0; index < dataLength; index++) {
    //   Serial.print(data[index]);
    // }
    Serial.println(data);
  }
}
