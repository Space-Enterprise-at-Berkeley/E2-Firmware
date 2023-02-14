#include "HAL.h"

namespace HAL {

    ESP32Encoder encoder;


    int init() {


        motorSPI = new SPIClass(VSPI);
        dataSPI = new SPIClass(HSPI);

        motorSPI->begin(MOTOR_SCLK, MOTOR_MISO, MOTOR_MOSI);
        dataSPI->begin(ETH_SCLK, ETH_MISO, ETH_MOSI);

        int motorDriverInitSuccess = initializeMotorDriver();
        if (motorDriverInitSuccess == -1) {
            disableMotorDriver();
            return -1;
        }
        
        pinMode(encA, INPUT);
        pinMode(encB, INPUT);

        encoder.attachFullQuad(encA, encB);
    }

    
    float readUpstreamPT() {
        #ifndef IS_INJECTOR
        return Ducers::readPressurantPT();
        #else
        return Ducers::readTankFromInjectorPT();
        #endif
    }

    float readDownstreamPT() {
        #ifndef IS_INJECTOR
        return Ducers::readTankPT();
        #else
        return Ducers::readInjectorPT();
        #endif
    }

    int initializeMotorDriver() {
        pinMode(DRV_CS, OUTPUT);
        pinMode(DRV_EN, OUTPUT);
        pinMode(DRV_FAULT, INPUT_PULLUP);
        pinMode(INHC, OUTPUT);
        pinMode(INLC, OUTPUT);

        digitalWrite(DRV_CS, HIGH);
        digitalWrite(INHC, LOW);
        digitalWrite(INLC, LOW);
        disableMotorDriver();

        attachInterrupt(DRV_EN, handleMotorDriverFault, FALLING);

        int pwmFreq = 50000;
        int pwmResolution = 8;
        ledcSetup(motorChannel, pwmFreq, pwmResolution);
        ledcAttachPin(INHA, 0);
        ledcWrite(INHA, 0);

        enableMotorDriver();
        delay(10);

        //set driver control
        readMotorDriverRegister(2);
        SPIBUFF[1] = 0b11000000; //1x pwm control for trap control
        writeMotorDriverRegister(2);
        readMotorDriverRegister(2);
        if (SPIBUFF[0] != 0x00 && SPIBUFF[1] != 0xc0) {
            Serial.printf("reg 2 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        } 

        //set CSA
        readMotorDriverRegister(6);
        SPIBUFF[1] = 0b10000001; //sense ocp 0.5v, gain 20v/v
        writeMotorDriverRegister(6);
        readMotorDriverRegister(6);
        if (SPIBUFF[0] != 0x02 && SPIBUFF[1] != 0x80) {
            Serial.printf("reg 6 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        } 

        // set gate drive HS 
        readMotorDriverRegister(3);
        SPIBUFF[1] = 0b11111111; //1000mA, 2000mA source/sink gate current
        writeMotorDriverRegister(3);
        readMotorDriverRegister(3);
        if (SPIBUFF[0] != 0x03 && SPIBUFF[1] != 0xFF) {
            Serial.printf("reg 3 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        }

        //set gate drive LS
        readMotorDriverRegister(4);
        SPIBUFF[1] = 0b11111111; //1000mA, 2000mA source/sink gate current
        writeMotorDriverRegister(4);
        readMotorDriverRegister(4);
        if (SPIBUFF[0] != 0x07 && SPIBUFF[1] != 0x84) {
            Serial.printf("reg 4 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        }

        //set OCP
        readMotorDriverRegister(5);
        SPIBUFF[1] = 0b01110101; // OC to auto retry under fault, OC deglitch to 8us, Vds trip to 0.45V
        writeMotorDriverRegister(5);
        readMotorDriverRegister(5);
        if (SPIBUFF[0] != 0x01 && SPIBUFF[1] != 0x35) {
            Serial.printf("reg 5 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        }

        disableMotorDriver();

        return 0;

    }

    void readMotorDriverRegister(int8_t addr) {
        SPIBUFF[0]  = 0x00;
        SPIBUFF[1] = 0x00;
        SPIBUFF[0] |= 0b10000000; //set read mode
        addr &= 0x0F;
        SPIBUFF[0] |= (addr << 3);
        sendSPICommand(SPIBUFF, 2, motorSPI, DRV_CS, DRVSPISpeed, SPI_MODE1);
    }

    void writeMotorDriverRegister(int8_t addr) {
        SPIBUFF[0] &= 0b01111111; //set write mode
        addr &= 0x0F;
        SPIBUFF[0] |= (addr << 3);
        sendSPICommand(SPIBUFF, 2, motorSPI, DRV_CS, DRVSPISpeed, SPI_MODE1);
    }

    void sendSPICommand(void* dataBuffer, int numBytes, SPIClass* spi, int csPin, int clkSpeed, int spiMode) {
        spi->beginTransaction(SPISettings(clkSpeed, MSBFIRST, spiMode));
        digitalWrite(csPin, LOW);
        spi->transfer(dataBuffer, numBytes);
        spi->endTransaction();
        digitalWrite(csPin, HIGH);
    }

    void enableMotorDriver() {
        digitalWrite(DRV_EN, HIGH);
    }

    void disableMotorDriver() {
        digitalWrite(DRV_EN, LOW);
    }

    void handleMotorDriverFault() {
        motorDriverFault = true;
        ledcWrite(motorChannel, 0);
        disableMotorDriver();
    }

    void printMotorDriverFault() {
        ledcWrite(motorChannel, 0);
        enableMotorDriver();
        delayMicroseconds(50);
        readMotorDriverRegister(0);
        Serial.printf("Fault:\n reg 0 <%hhx>, <%hhx>\n", SPIBUFF[0], SPIBUFF[1]);
        readMotorDriverRegister(1);
        Serial.printf("reg 0 <%hhx>, <%hhx>\n", SPIBUFF[0], SPIBUFF[1]);
        disableMotorDriver();

    }

    float readADC(SPIClass* spi, uint8_t csPin, int8_t channel) {
        if ((channel > 3) || (channel < 0)) {
            DEBUGF("bad channel index\n");
            return 0;
        }
        SPIBUFF[0] = 0;
        SPIBUFF[0] |= (channel << 3);
        SPIBUFF[0] &= 0b00011000;
        SPIBUFF[1] = 0;
        sendSPICommand(SPIBUFF, 2, spi, csPin, ADCSPISpeed, SPI_MODE0);

        int16_t val = ((SPIBUFF[0] & 0b00001111) << 8) + SPIBUFF[1];
        
        float f = (((float) val) / 4096) * 5.0;
        f = (5 - f) / (20*0.005);
        return f;
    }  

    float readPhaseCurrent(uint8_t phase) {
        if ((phase > 2) || (phase < 0)) {
            DEBUGF("bad motor current phase index\n");
            return 0;
        }
        return readADC(motorSPI, MADC_CS, phase);
    }
    
    float readPTVoltage(uint8_t channel) {
        if ((channel > 3) || (channel < 0)) {
            DEBUGF("bad channel index\n");
            return 0; 
        }
        return readADC(dataSPI, PTADC_CS, channel);
    }

}