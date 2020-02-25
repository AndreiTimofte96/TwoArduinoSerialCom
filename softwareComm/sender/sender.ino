
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(38400);
  delay(1000);
}

int ok = true;
char x[] = "12345\f7";

void loop()  // run over and over
{
  if (ok) {
    mySerial.write(x, strlen(x));
    Serial.write(x, strlen(x));
    ok = false;
  }
}
