#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "Util.h" 
#include "SPI.h"
#include "Common.h"
// #include "Packets.h" // Does not exist
#include "TeensyComms.h"

namespace HAL {

    const int motorChannel = 0; // pin for pwm-ing motors

    // Best encoder Performance: both pins have interrupt capability
    // avoid using pins with LEDs attached

    //for now, only uses two encoders similar to quadrature. Less fine position control, meh

    const int encA = 14; //encoder A
    const int encB = 15; //encoder B
    const int encC = 16; //encoder C


    // const int LIMIT_1 = 40;
    // const int LIMIT_2 = 41;

    const int SPARKMAX = 42;


    int init();

    int initializeMotorDriver();

    void enableMotorDriver();
    void disableMotorDriver();

    void setEncoderCount(int i);
    int getEncoderCount();
    void setupEncoder();

    // int getOpenLimitSwitchState();
    // int getClosedLimitSwitchState();
}
