#pragma once

#include <Common.h>

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
namespace HAL { 
    #define RS485_SERIAL Serial1;

    const uint8_t RS485_RECEIVE = 36;
    const uint8_t RS485_TRANSMIT = 37;

    const uint8_t gpsCSPin = 38;

    void initHAL();
}