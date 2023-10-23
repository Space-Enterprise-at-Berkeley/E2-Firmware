// MAIN RS485
#include <Common.h>
#include <EspComms.h>

#include <Arduino.h>

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  Serial.begin(115200);

// changes
  Serial1.begin(115200);
  Serial1.setPins(17, 18);
}

void loop() {
  Serial.println("lol");
  delay(200);
}
