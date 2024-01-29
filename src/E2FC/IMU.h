#pragma once

#include <Arduino.h>
#include <EspComms.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DSO32.h>
#include <SparkFun_KX13X.h>

namespace IMU
{

    void init();
    uint32_t updateFlight();

}