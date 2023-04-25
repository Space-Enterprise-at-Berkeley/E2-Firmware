#include <Arduino.h>
#include "BlackBox.h"
#include <EspComms.h>

Comms::Packet packet = {.id = 5};

void setup(){
    // Comms::init();

    delay(10000);

    Serial.begin(921600);
    Serial.println("Initializing....");
    BlackBox::init();

    Serial.println("booting");

    // #ifdef ERASE
    //     BlackBox::erase();
    //     Serial.println("erase complete");
    // #else
    // write 1000 packets to blackbox
    for(uint32_t i = 0; i < 1000; i++) {
        packet.len = 0;
        Comms::packetAddUint32(&packet, i);
        BlackBox::writePacket(&packet);
        // Serial.println("hello, written a thing");
    }

    // read 1000 packets from blackbox and check if they have correct data

    for (uint32_t i = 0; i < 12 * 1000; i += 12) {
        packet = BlackBox::getData(i);
        uint32_t data = Comms::packetGetUint32(&packet, 0);
        Serial.printf("Blackbox packet %i has data %i \n", i/12, data);
    }
    // #endif
}

void loop() {
    
}