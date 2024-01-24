#pragma once

#include <Common.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MuxChannel.h> // check

#include <INA233.h> // INA233AIDGST VIN Power Monitor
// W25Q128JVSIQ Black Box // check
#include <ADS8167.h> // ADS8167IRHBT ADC // check
#include <Adafruit_BMP3XX.h> // BMP390 Barometer
#include <MS5xxx.h> // MS560702BA03-50 Fancy Barometer
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // NEO-M9N-00B GPS
#include <Adafruit_LSM6DSO32.h> // LSM6DSO32TR Accelerometer
#include <SparkFun_KX13X.h> // KX134-1211 High-G Accelerometer
// CMP-00025-00005-1 Radio // check

#include <Adafruit_Sensor.h> // check
#include <MAX31865.h> // RTDs? check
#include <MCP23008.h> // IO expander? check

namespace HAL {
    // Serial
    #define RADIO_SERIAL Serial0
    #define RS422_SERIAL Serial1

    // Sensors
    extern INA233 ina(INA233_ADDRESS_41, Wire); // check
    extern ADS8167 adc; // check
    extern Adafruit_BMP3XX bmp;
    extern MS5xxx ms5607(&Wire); // check
    extern SFE_UBLOX_GNSS neom9n;
    extern Adafruit_LSM6DSO32 dso32;
    extern SparkFun_KX134 kx134;

    extern SPIClass spi0; // check
    extern MCP23008 MCP0(0x27); // check
    extern MCP23008 MCP1(0x20); // check

    // ESP Pin Mappings
    const uint8_t IO0 = 0;

    const uint8_t SDA = 1;
    const uint8_t SCL = 2;

    const uint8_t MUX_OUT_0 = 3;
    const uint8_t MUX_OUT_0 = 4;
    const uint8_t MUX_S2 = 5;
    const uint8_t MUX_S1 = 6;
    const uint8_t MUX_S0 = 7;

    const uint8_t VIDEO = 8;

    const uint8_t ETHSPI_INTn = 9;
    const uint8_t ETHSPI_CS = 10;
    const uint8_t ETHSPI_MOSI = 11;
    const uint8_t ETHSPI_CLK = 12;
    const uint8_t ETHSPI_MISO = 13;

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

    const uint8_t RX1 = 36; // RS422 EReg
    const uint8_t TX1 = 37; // RS422 EReg

    const uint8_t RX0 = 38; // Radio
    const uint8_t TX0 = 39; // Radio

    const uint8_t ADC_CS = 40;

    const uint8_t MOSI = 41;
    const uint8_t MISO = 42;
    const uint8_t SCK = 45;

    const uint8_t ADC_RDY = 46;

    const uint8_t GEMS1 = 47;

    void initHAL();
}
