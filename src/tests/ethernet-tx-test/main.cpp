#include <Arduino.h>
#include "EspComms.h"

void setup()
{
  Serial.begin(921600);
  Comms::init();
}

char packetBuffer[sizeof(Packet)];


void loop()
{
  // Serial.println("before begin");
  // Serial.flush();
  if (Ethernet.detectRead()) {
      if (Udp.parsePacket()) {
        Serial.println("packet");
        // if(Udp.remotePort() != port) return;
        Udp.read(packetBuffer, sizeof(Comms::Packet));
        Packet *packet = (Packet*) &packetBuffer;
        evokeCallbackFunction(packet, Udp.remoteIP()[3]);
        
      }
  }

  // Serial.println(count);
  // Serial.flush();
  // count += 1;

  // delay(1000);
}
