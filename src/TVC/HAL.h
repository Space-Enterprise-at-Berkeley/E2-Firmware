#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "SPI.h"
#include "Common.h"
// #include "Packets.h" // Does not exist
#include "EspComms.h"

namespace HAL {

    const int x_pwm = 39;
    const int y_pwm = 7;

    // Best encoder Performance: both pins have interrupt capability
    // avoid using pins with LEDs attached

    const int encA = 19; //encoder A orange
    const int encB = 18; //encoder B grey
    const int encC = 21; //encoder C yellow

    extern volatile int encoderTicks;


    int init();

    void setEncoderCount(int i);
    int getEncoderCount();
    void setupEncoder();
    uint32_t printEncoder();
}
