#include "HAL.h"

namespace HAL {
    ADS8167 adc1;

    // //Voltage and Current Sense Mux
    // Mux valveMux = {.muxSelect1Pin = 3,
    //                 .muxSelect2Pin = 4,
    //                 .muxSelect3Pin = 5,
    //                 .muxSelect4Pin = 6,
    //                 .muxInput1Pin = 40,
    //                 .muxInput2Pin = 16};

    // MuxChannel muxChan0; // Chute1
    // MuxChannel muxChan1; // Chute2
    // MuxChannel muxChan2; // Cam1/NA
    // MuxChannel muxChan3; // Amp/NA
    // MuxChannel muxChan4; // Cam2/Break1
    // MuxChannel muxChan5; // Radio/Break2
    // MuxChannel muxChan6; // NA/Break3
    // MuxChannel muxChan7; // Valve1
    // MuxChannel muxChan8; // Valve2
    // MuxChannel muxChan9; // Valve3
    // MuxChannel muxChan10; // Valve4
    // MuxChannel muxChan11; // Valve5
    // MuxChannel muxChan12; // Valve6
    // MuxChannel muxChan13; // HBridge1
    // MuxChannel muxChan14; // HBridge2
    // MuxChannel muxChan15; // HBridge3

    // MCP9600 tcAmp0;
    // MCP9600 tcAmp1;
    // MCP9600 tcAmp2;
    // MCP9600 tcAmp3;

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
