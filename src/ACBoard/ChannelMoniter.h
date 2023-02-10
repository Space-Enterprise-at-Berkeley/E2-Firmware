#pragma once

#include <Arduino.h>
#include <EspComms.h>;


namespace ChannelMoniter {

    uint8_t sel0, sel1, sel2, currpin, contpin;

    void init(uint8_t sel0, uint8_t sel1, uint8_t sel2, uint8_t currpin, uint8_t contpin);

    uint32_t cmUpdatePeriod;

    uint32_t readChannels();
    


}