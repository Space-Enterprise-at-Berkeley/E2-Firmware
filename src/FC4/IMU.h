#pragma once

#include "HAL.h"
#include <EspComms.h>
#include <Arduino.h>

namespace IMU {
    extern uint32_t imuUpdatePeriod;

    extern float qW;
    extern float qX;
    extern float qY;
    extern float qZ;
    extern float accelX;
    extern float accelY;
    extern float accelZ;

    void initIMU();
    uint32_t imuSample();

    float get_acceleration_x();
    float get_acceleration_y();
    float get_acceleration_z();

    float get_gyro_x();
    float get_gyro_y();
    float get_gyro_z();

};