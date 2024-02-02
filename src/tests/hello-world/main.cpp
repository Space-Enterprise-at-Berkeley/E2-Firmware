
#include <MAX22201.h>

MAX22201 m;

void setup() {
  m.init(8, 255);
  Serial.begin(921600);
}

void loop() {
  Serial.println("going");
  delay(2000);
  m.forwards();
  delay(2000);
  m.stop();
}
