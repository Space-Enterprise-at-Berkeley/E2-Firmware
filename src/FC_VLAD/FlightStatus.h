#pragma once

#include <Arduino.h>
#include <WiFiComms.h>
#include <ArduinoEigenDense.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_LSM6DSO32.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <MS5xxx.h>
#include <SparkFun_KX13X.h>
#include "Radio.h"

namespace FlightStatus
{

    void init();
    uint32_t updateFlight();

}