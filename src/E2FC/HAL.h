#pragma once

#include <Common.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
namespace HAL { 
    #define RS485_SERIAL Serial1;

    const uint8_t RS485_RECEIVE = 36;
    const uint8_t RS485_TRANSMIT = 37;

    void initHAL();
}