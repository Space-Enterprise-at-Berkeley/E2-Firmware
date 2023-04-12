#pragma once

#include "HAL.h"
#include <EspComms.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DSO32.h>
#include <SparkFun_KX13X.h> 

namespace IMU {

    Adafruit_LSM6DSO32 imu;
    SparkFun_KX134 imu_highg;
    
    extern uint32_t imuUpdatePeriod;

    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    
    extern float accelX_high;
    extern float accelY_high;
    extern float accelZ_high;

    void initIMU();
    uint32_t imuSample();

    void initIMUHighG();
    uint32_t imuHighGSample();

    float get_acceleration_x();
    float get_acceleration_y();
    float get_acceleration_z();

    float get_gyro_x();
    float get_gyro_y();
    float get_gyro_z();

    float get_accel_x_highg();
    float get_accel_y_highg();
    float get_accel_z_highg();

};