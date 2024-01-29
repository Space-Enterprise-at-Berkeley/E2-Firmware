#include <Arduino.h>
#include "EspComms.h"

void setup()
{
  Serial.begin(921600);
  Comms::init();
}

void loop()
{
  // Serial.println("before begin");
  // Serial.flush();
  static int cnt = 0;
  while(1) {
    Serial.println("sending packet");
    Comms::Packet tmp = {.id = 2};
    Comms::packetAddFloat(&tmp, 1.0);
    Comms::packetAddFloat(&tmp, 1.0);
    Comms::packetAddFloat(&tmp, 1.0);
    Comms::emitPacketToGS(&tmp);
    cnt++;
    delay(100);
  }

  // Serial.println(count);
  // Serial.flush();
  // count += 1;

  // delay(1000);
}
