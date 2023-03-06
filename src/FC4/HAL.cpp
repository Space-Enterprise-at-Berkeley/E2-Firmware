#include "HAL.h"

namespace HAL {
    ADS8167 adc1;

    // // Sensors breakouts
    // BMP388_DEV bmp388;
    // BNO055 bno055(28);
    // SFE_UBLOX_GNSS neom9n;

    void initHAL() {
        // initialize ADC 1
        analogReadResolution(12); 

        adc1.init(&SPI, 37, 25);
        adc1.setAllInputsSeparate();
        adc1.enableOTFMode();

        // Initialize I2C buses
        Wire.begin();
        Wire.setClock(100000);

        // // barometer
        // bmp388.begin(0x76); // TODO check address

        // // imu
        // bno055.begin();

        // // gps
        // if(!neom9n.begin(SPI, gpsCSPin, 2000000)) {
        //     DEBUG("GPS DIDN'T INIT");
        //     DEBUG("\n");
        // } else {
        //     DEBUG("GPS INIT SUCCESSFUL");
        //     DEBUG("\n");
        // }

    }
};
