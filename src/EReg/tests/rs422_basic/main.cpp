#include <Arduino.h>
#include <Wire.h>
#include <EspComms.h>

int rx=17;
int tx=18;

void setup() {
  Serial.begin(921600);
  // setup stuff here
  Serial1.begin(921600, SERIAL_8N1, rx, tx);

  while (1) {

    Serial1.write(0x69);
    delay(20);
    Serial.write(0x69);

  }

}

void loop() {}