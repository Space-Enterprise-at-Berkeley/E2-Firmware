
#include <MAX22201.h>

MAX22201 m;

void setup() {
  Serial.begin(921600);
  pinMode(9, OUTPUT);
}

void loop() {
  delay(500);
  digitalWrite(9, HIGH);
  delay(500);
  digitalWrite(9, LOW);
}
