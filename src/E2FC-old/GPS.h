#pragma once

#include <Arduino.h>
#include <EspComms.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

namespace FlightStatus
{

    void init();
    uint32_t updateFlight();

}