#include <Arduino.h>
#include <Wire.h>
#include <EspComms.h>

char oregbuffer[sizeof(Comms::Packet)];
uint8_t ocnt = 0;
char fregbuffer[sizeof(Comms::Packet)];
uint8_t fcnt = 0;

void setup() {
    Serial.begin(921600);
  // setup stuff here
  Serial1.begin(115200, SERIAL_8N1, 38, 39);
  Serial2.begin(115200, SERIAL_8N1, 36, 37);

  while (1) {

    while (Serial1.available()) {
        Serial.print()
    }

  }

}

void loop() {}