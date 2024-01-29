#pragma once

#include <Arduino.h>
#include <EspComms.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <MS5xxx.h>

namespace Barometer
{

    void init();
    uint32_t updateFlight();

}