#pragma once

#include <Common.h>
#include <EspComms.h>
#include <ADS8167.h>

#include <Arduino.h>

namespace Ducers {
    void init();
    float samplePT(uint8_t channel);
    float noSamplePT(uint8_t channel);
    uint32_t task_ptSample();
    void print_ptSample();
    float zeroChannel(uint8_t channel);
    float calChannel(uint8_t channel, float value);
};