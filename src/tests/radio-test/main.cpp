#include <Wire.h>
#include <Common.h>
#include <EspComms.h>
#include <Arduino.h>
#include <SPI.h>
#include "Radio.h"


void setup() {
    Serial.begin(921600);






    Comms::init();
    Radio::initRadio();




}

void loop()
{
    Comms::Packet imuPacket = {.id = 3};
    imuPacket.len = 0;
    Comms::packetAddFloat(&imuPacket, 5.5f);
    Comms::emitPacketToGS(&imuPacket);
    Radio::forwardPacket(&imuPacket);
}

