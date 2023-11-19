#pragma once

#include <Common.h>

#include <ADS8167.h> // ADC
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // GPS
#include "Adafruit_BMP3XX.h" // Barometer
#include <INA233.h> // Power
#include <Adafruit_LSM6DSO32.h> // Accelerometer
#include <SparkFun_KX13X.h> // High-G Accelerometer
#include <MS5xxx.h> // Fancy Barometer

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
namespace HAL { 
    // Serial
    #define RS485_SERIAL1 Serial1;
    #define RS485_SERIAL0 Serial0;

    // Breakouts
    extern ADS8167 ads8167; // ADC, SPI
    extern SFE_UBLOX_GNSS neom9n; // check library GPS, I2C
    extern Adafruit_LSM6DSO32 lsm6dso32; // Accelerometer, I2C
    extern SparkFun_KX134 kx134; // High-G Accelerometer, I2C
    extern Adafruit_BMP3XX bmp390; // Barometer, I2C
    extern MS5xxx ms5607; // Fancy Barometer, I2C // i think this works lol
    extern INA233 ina233; // Power, I2C

    // ESP pin mappings
    const uint8_t SDA = 1;
    const uint8_t SCL = 2;

    const uint8_t MUX_OUT0 = 3;
    const uint8_t MUX_OUT1 = 4;
    const uint8_t MUX_S2 = 5;
    const uint8_t MUX_S1 = 6;
    const uint8_t MUX_S0 = 7;

    const uint8_t VIDEO = 8;

    const uint8_t ETHERNET_INTn = 9;
    const uint8_t ETHERNET_CS = 10;
    const uint8_t ETHERNET_MOSI = 11;
    const uint8_t ETHERNET_CLK = 12;
    const uint8_t ETHERNET_MISO = 13;

    const uint8_t RADIO_CS = 14;
    const uint8_t RADIO_SDN = 15;
    const uint8_t RADIO_MOSI = 16;
    const uint8_t RADIO_MISO = 17;
    const uint8_t RADIO_SCK = 18;

    const uint8_t BB_CS = 19;
    const uint8_t BB_MOSI = 20;
    const uint8_t BB_MISO = 21;
    const uint8_t BB_SCK = 26;

    const uint8_t GEMS2 = 33;

    const uint8_t RBV_IN1 = 34;
    const uint8_t RBV_IN2 = 35;

    const uint8_t RS485_RECEIVE1 = 36; // EReg
    const uint8_t RS485_TRANSMIT1 = 37; //EReg
    const uint8_t RS485_RECEIVE0 = 38;
    const uint8_t RS485_TRANSMIT0 = 39;

    const uint8_t ADC_CS = 40;
    const uint8_t MOSI = 41;
    const uint8_t MISO = 42;
    const uint8_t SCK = 45;
    const uint8_t ADC_RDY = 46;

    const uint8_t GEMS1 = 47;

    // Methods
    void initHAL();
}