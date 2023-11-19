#include "HAL.h"

namespace HAL {

    ADS8167 ads8167; // ADC, SPI
    SFE_UBLOX_GNSS neom9n; // check library GPS, I2C
    Adafruit_LSM6DSO32 lsm6dso32; // Accelerometer, I2C
    SparkFun_KX134 kx134; // High-G Accelerometer, I2C
    Adafruit_BMP3XX bmp390; // Barometer, I2C
    MS5xxx ms5607(&Wire); // Fancy Barometer, I2C
    INA233 ina233(INA233_ADDRESS_41, Wire); // Power, I2C

    void initHAL() {
        
    }
}