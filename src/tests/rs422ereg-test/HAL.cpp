#include "HAL.h"

namespace HAL {

    void initHAL() {

        pinMode(adcCS, OUTPUT);
        pinMode(radioCS, OUTPUT);
        pinMode(bbCS, OUTPUT);

        // initialize SPI
        spi0 = new SPIClass(HSPI);
        spi0->begin(sckPin, misoPin, mosiPin, adcCS);
        spi1 = new SPIClass(FSPI);

        // initialize I2C
        Wire.begin(sdaPin, sclPin); 
        Wire.setClock(100000);


        // initialize ADC
        adc.init(spi0, adcCS, adcRDY);
        adc.setAllInputsSeparate();
        adc.enableOTFMode();

    }
};
