#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "SPI.h"
#include "Common.h"
// #include "Packets.h" // Does not exist
#include "EspComms.h"

namespace HAL {


// #define X_PWM_PIN 6
// #define Y_PWM_PIN 7

    const int x_pwm = 6;
    const int y_pwm = 7;

    // Best encoder Performance: both pins have interrupt capability
    // avoid using pins with LEDs attached

    //for now, only uses two encoders similar to quadrature. Less fine position control, meh

    const int encA = 27; //encoder A
    const int encB = 25; //encoder B
    const int encC = 24; //encoder C

    extern volatile int encoderTicks;


    int init();

    void setEncoderCount(int i);
    int getEncoderCount();
    void setupEncoder();
    uint32_t printEncoder();

    // int getOpenLimitSwitchState();
    // int getClosedLimitSwitchState();
}
