    #include <Arduino.h>
    #include <SPI.h>
    #include "pins.h"

    int ADCSPISpeed = 3000000;
    // uint8_t SPIBUFF[10]; // spi buffer for all SPI except ethernet.
    SPIClass *dataSPI = NULL;


    void sendSPICommand(uint8_t* dataBuffer, int numBytes, SPIClass* spi, int csPin, int clkSpeed, int spiMode) {
        spi->beginTransaction(SPISettings(clkSpeed, MSBFIRST, spiMode));
        spi->transfer(0x00, 1);
        spi->endTransaction();
        spi->beginTransaction(SPISettings(clkSpeed, MSBFIRST, spiMode));
        digitalWrite(csPin, LOW);
        spi->transfer(dataBuffer, numBytes);
        spi->endTransaction();
        digitalWrite(csPin, HIGH);
    }

    uint8_t SPIBUFF[4];
    float pts[4];

    float readIndivChannel(SPIClass *spi, uint8_t csPin, uint8_t channel) {
        SPIBUFF[0] = 0;
        SPIBUFF[0] |= (channel << 3);
        // SPIBUFF[0] &= 0b00011000;
        SPIBUFF[1] = 0;
        sendSPICommand(SPIBUFF, 4, spi, csPin, ADCSPISpeed, SPI_MODE0);
        uint16_t val = 0;
        val = ((SPIBUFF[2] & 0b00001111) << 8) + SPIBUFF[3];
        float f = (((float) val) / 4096.0) * 5.0;
        return f;

    }
    void readADC(SPIClass* spi, uint8_t csPin) {
        
        for (uint8_t i = 0; i < 4; i++) {
            SPIBUFF[2*i] = 0;
            SPIBUFF[2*i] |= (i << 3);
            SPIBUFF[2*i] &= 0b00011000;
            SPIBUFF[(2*i) + 1] = 0;
        }

        sendSPICommand(SPIBUFF, 10, spi, csPin, ADCSPISpeed, SPI_MODE0);
        for (int i = 0; i < 4; i++) {
            uint16_t val = 0;
            val = ((SPIBUFF[2*(i+1)] & 0b00001111) << 8) + SPIBUFF[(2*(i+1)) + 1];
            float f = (((float) val) / 4096.0) * 5.0;
            pts[i] = f;
        }
    }  



    void setup() {
        Serial.begin(115200);
        dataSPI = new SPIClass(FSPI);

        pinMode(PTADC_CS, OUTPUT);
        digitalWrite(PTADC_CS, HIGH);
        dataSPI->begin(ETH_SCLK, ETH_MISO, ETH_MOSI);
    }

    void loop() {
        // readADC(dataSPI, PTADC_CS);
        pts[0] = readIndivChannel(dataSPI, PTADC_CS, 0);
        pts[1] = readIndivChannel(dataSPI, PTADC_CS, 1);
        pts[2] = readIndivChannel(dataSPI, PTADC_CS, 2) * 2.0084; //5.6K
        pts[3] = readIndivChannel(dataSPI, PTADC_CS, 3) * 2.0084;

        Serial.printf("output voltages: %f, %f, %f, %f\n", pts[0], pts[1], pts[2], pts[3]);
        // Serial.printf("output voltage: %f, %f\n", b, d);
        delay(200);
    }