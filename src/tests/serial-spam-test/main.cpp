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
  Serial1.begin(921600, SERIAL_8N1, 38, 39); //oreg
  Serial2.begin(921600, SERIAL_8N1, 36, 37); //freg
  // FREG_SERIAL.begin(921600, SERIAL_8N1, rx1P, tx1P)

  while (1) {

    while (Serial2.available()) {
        Serial.print(Serial2.read());
    }

  }

}

void loop() {}