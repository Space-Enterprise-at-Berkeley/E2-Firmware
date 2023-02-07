#include "ChannelMoniter.h";
#include <EspComms.h>;

namespace ChannelMoniter {

uint32_t cmUpdatePeriod = 1000 * 1000;

uint8_t sel0, sel1, sel2, currpin, contpin;

void init(uint8_t sel0, uint8_t sel1, uint8_t sel2, uint8_t currpin, uint8_t contpin){
    sel0 = sel0;
    sel1 = sel1;
    sel2 = sel2;
    currpin = currpin;
    contpin = contpin;

    pinMode(sel0, OUTPUT);
    pinMode(sel1, OUTPUT);
    pinMode(sel2, OUTPUT);
    pinMode(currpin, INPUT);
    pinMode(contpin, INPUT);
}

uint32_t readChannels() {
    Comms::Packet contPacket = {.id = 3};
    Comms::Packet currPacket = {.id = 4};

    for (int i = 0; i < 8; i ++){
        digitalWrite(sel0, i & 0x01);
        digitalWrite(sel1, (i >> 1) & 0x01);
        digitalWrite(sel2, (i >> 2) & 0x01);

        Comms::packetAddFloat(&contPacket, analogRead(contpin));
        Comms::packetAddFloat(&currPacket, analogRead(currpin));
    }  
    return cmUpdatePeriod;
}
}