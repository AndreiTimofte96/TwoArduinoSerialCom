
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  mySerial.begin(38400);
}

void readBytes(char *data, int length) {
  int index;
  String str;

  for (int index = 0; index < length; index++) {
    char c = mySerial.read();
    str += c;
  }
  strcpy(data, str.c_str());
}

char *data;
int dataLength = 7;

void loop()  // run over and over
{
  while (!mySerial.available())
    ;

  data = (char *)malloc(sizeof(char) * dataLength + 1);
  memset(data, '\0', sizeof(char) * dataLength + 1);

  readBytes(data, dataLength);
  Serial.println(data);

  delay(100000000);
}
