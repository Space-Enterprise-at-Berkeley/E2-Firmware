#pragma once

#include <Common.h>

#include <ADS8167.h>
#include <MAX31865.h>
#include <MCP23008.h>
#include <Adafruit_LSM6DSO32.h>
#include <SparkFun_Qwiic_KX13X.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
namespace HAL {   

    #define RADIO_SERIAL Serial0
    #define OREG_SERIAL Serial1 
    #define FREG_SERIAL Serial2
        
    // SPI
    const uint8_t mosiPin = 6;
    const uint8_t misoPin = 7;
    const uint8_t sckPin = 8;

    extern SPIClass* spi0;
    extern SPIClass* spi1;

    // I2C
    const uint8_t sdaPin = 1;
    const uint8_t sclPin = 2;

    // Serial 
    const uint8_t tx0P = 35;
    const uint8_t rx0P = 34;
    
    const uint8_t tx1P = 33;
    const uint8_t rx1P = 26;

    const uint8_t tx2P = 21;
    const uint8_t rx2P = 20;

    // Flight data
    extern Adafruit_LSM6DSO32 ds032;


    // Valves, video, chutes, etc. 
    // MCP0
    const uint8_t valve0Pin = 0;
    const uint8_t valve1Pin = 1;
    const uint8_t valve2Pin = 2;
    const uint8_t valve3Pin = 3;
    const uint8_t chute0Pin = 4;
    const uint8_t chute1Pin = 5;
    const uint8_t video0Pin = 7;
    const uint8_t video1Pin = 6;

    // MCP1
    const uint8_t valve4Pin = 4;
    const uint8_t rbv0Pin = 7;
    const uint8_t rbv1Pin = 6;


    // Multiplexer
    const uint8_t muxS0 = 40;
    const uint8_t muxS1 = 39;
    const uint8_t muxS2 = 38;
    const uint8_t contPin = 41;
    const uint8_t currPin = 42;
    const uint8_t expMuxPin = 36;

    // ADC
    const uint8_t adcRDY = 4;
    const uint8_t adcCS = 47;

    extern ADS8167 adc;

    // Radio
    const uint8_t radioCS = 3;
    const uint8_t radioSDN = 9;

    // Flash chip
    const uint8_t bbCS = 37;

    // RTD
    const uint8_t rtd0CS = 45;
    const uint8_t rtd1CS = 46;

    void initHAL();
};