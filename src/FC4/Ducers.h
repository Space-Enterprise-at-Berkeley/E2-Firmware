#pragma once

#include <Common.h>
#include <EspComms.h>
#include "HAL.h"
#include <Arduino.h>

namespace Ducers {

    // extern uint32_t ptUpdatePeriod;

    extern float loxInjectorPTValue;
    extern float fuelInjectorPTValue;
    
    void init();
    // float interpolate1000(uint16_t rawValue);
    // float interpolate5000(uint16_t rawValue);
    float samplePT(uint8_t channel);
    float noSamplePT(uint8_t channel);
    uint32_t task_ptSample();
    void print_ptSample();
    void zeroChannel(uint8_t channel);
};
