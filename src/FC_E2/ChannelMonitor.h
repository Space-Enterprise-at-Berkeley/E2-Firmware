#pragma once
#include <Arduino.h>
#include <MCP23008.h>
#include "WiFiCommsLite.h"
#include "Radio.h"

namespace ChannelMonitor {

    void init(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t curr, uint8_t cont);

    uint32_t task_readChannels();
    
    float* getCurrents();
    float* getContinuities();
    bool isChannelContinuous(uint8_t channel);
}