#include <Arduino.h>

#include "Comms.h"
#include "Common.h"
 
uint8_t LED = 18;

void setup()
{
  Serial.begin(921600);
  Comms::initComms();

  pinMode(LED, OUTPUT);
}
 
unsigned long previousMillis = 0;  
const long interval = 1000;

Comms::Packet capPacket = {.id = 220};

void loop()
{
  digitalWrite(LED, HIGH);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    capPacket.len = 0;

    //Serial.println(capValue);
    Comms::packetAddFloat(&capPacket, 420.69);
    Comms::emitPacket(&capPacket);
  }
  digitalWrite(LED, LOW);
}