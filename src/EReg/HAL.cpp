#include "HAL.h"

namespace HAL {

    int DRVSPISpeed = 1000000;
    int ADCSPISpeed = 1000000;
    volatile bool motorDriverFault = false;
    int8_t SPIBUFF[2]; // spi buffer for all SPI except ethernet.
    SPIClass *motorSPI = NULL;
    SPIClass *dataSPI = NULL;
    volatile int encoderTicks = 0;

    int init() {


        motorSPI = new SPIClass(FSPI);
        // dataSPI = new SPIClass(HSPI);


        motorSPI->begin(MOTOR_SCLK, MOTOR_MISO, MOTOR_MOSI);
        pinMode(MADC_CS, OUTPUT);
        digitalWrite(MADC_CS, HIGH);
        // dataSPI->begin(ETH_SCLK, ETH_MISO, ETH_MOSI);

        setupEncoder();


        int motorDriverInitSuccess = initializeMotorDriver();
        if (motorDriverInitSuccess == -1) {
            disableMotorDriver();
            return -1;
        }
        return 0;
    }

    
    int initializeMotorDriver() {
        Serial.printf("motor driver init start\n");
        pinMode(DRV_CS, OUTPUT);
        pinMode(DRV_EN, OUTPUT);
        pinMode(DRV_FAULT, INPUT_PULLUP);
        pinMode(INHC, OUTPUT);
        pinMode(INLC, OUTPUT);

        disableMotorDriver();
        motorDriverFault = false;
        digitalWrite(DRV_CS, HIGH);
        digitalWrite(INHC, LOW);
        digitalWrite(INLC, LOW);
        delay(10);


        int pwmFreq = 50000;
        int pwmResolution = 8;
        ledcSetup(motorChannel, pwmFreq, pwmResolution);
        ledcAttachPin(INHA, 0);
        ledcWrite(motorChannel, 0);

        enableMotorDriver();
        delay(10);
        printMotorDriverFaultAndDisable();
        enableMotorDriver();
        delay(10);
        attachInterrupt(DRV_FAULT, handleMotorDriverFault, FALLING);

        //set driver control
        SPIBUFF[0] = 0b00000000;
        SPIBUFF[1] = 0b11000000; //1x pwm control for trap control
        writeMotorDriverRegister(2);

        //set CSA
        SPIBUFF[0] = 0b00000010;
        SPIBUFF[1] = 0b10000001; //sense ocp 0.5v, gain 20v/v //TODO change this
        writeMotorDriverRegister(6);


        //set gate drive LS
        SPIBUFF[0] = 0b00000111;
        SPIBUFF[1] = 0b11101110; //1000mA, 2000mA source/sink gate current
        writeMotorDriverRegister(4);

        //set OCP
        SPIBUFF[0] = 0b00000001;
        SPIBUFF[1] = 0b01110101; // OC to auto retry under fault, OC deglitch to 8us, Vds trip to 0.45V
        writeMotorDriverRegister(5);

        // set gate drive HS 
        SPIBUFF[0] = 0b00000011;
        SPIBUFF[1] = 0b11101110; //1000mA, 2000mA source/sink gate current
        writeMotorDriverRegister(3);

        readMotorDriverRegister(2);
        if ((SPIBUFF[0] != (int8_t) 0x00) || (SPIBUFF[1] != (int8_t) 0xc0)) {
            Serial.printf("reg 2 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        } else {
            Serial.printf("reg 2 good!\n");
        }
        // delay(10);

        readMotorDriverRegister(6);
        if ((SPIBUFF[0] != (int8_t) 0x02) || (SPIBUFF[1] != (int8_t) 0x81)) {
            Serial.printf("reg 6 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        } else {
            Serial.printf("reg 6 good!\n");
        } 
        // delay(10);

        readMotorDriverRegister(3);
        if ((SPIBUFF[0] != (int8_t) 0x03) || (SPIBUFF[1] != (int8_t) 0xEE)) {
            Serial.printf("reg 3 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        } else {
            Serial.printf("reg 3 good!\n");
        }
        // delay(10);


        readMotorDriverRegister(4);
        if ((SPIBUFF[0] != (int8_t) 0x07) || (SPIBUFF[1] != (int8_t) 0xEE)) {
            Serial.printf("reg 4 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        } else {
            Serial.printf("reg 4 good!\n");
        }
        // printMotorDriverFaultAndDisable();

        readMotorDriverRegister(5);
        if ((SPIBUFF[0] != (int8_t) 0x01) || (SPIBUFF[1] != (int8_t) 0x75)) {
            Serial.printf("reg 5 bad :( %hhx, %hhx\n", SPIBUFF[0], SPIBUFF[1]);
            return -1;
        } else {
            Serial.printf("reg 5 good!\n");
        }
        // disableMotorDriver(); //remove this later
        return 0;

    }

    void readMotorDriverRegister(int8_t addr) {
        SPIBUFF[0]  = 0x00;
        SPIBUFF[1] = 0x00;
        SPIBUFF[0] |= 0b10000000; //set read mode
        addr &= 0x0F;
        SPIBUFF[0] |= (addr << 3);
        sendSPICommand(SPIBUFF, 2, motorSPI, DRV_CS, DRVSPISpeed, SPI_MODE1);
        SPIBUFF[0] &= 0b00000111;
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
    }

    void printMotorDriverFaultAndDisable() {
        ledcWrite(motorChannel, 0);
        delayMicroseconds(50);
        readMotorDriverRegister(0);
        Serial.printf("Fault:\n reg 0 <%hhx>, <%hhx>\n", SPIBUFF[0], SPIBUFF[1]);
        readMotorDriverRegister(1);
        Serial.printf("reg 1 <%hhx>, <%hhx>\n", SPIBUFF[0], SPIBUFF[1]);
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
        return 1;
        // return readADC(dataSPI, PTADC_CS, channel);
    }

    float readUpstreamPT() {
        return Ducers::readPressurantPT();
    }

    float readDownstreamPT() {
        return Ducers::readTankPT();
    }

    void clearMotorDriverFault() {
        motorDriverFault = false;
    }

    bool getMotorDriverFault() {
        return motorDriverFault;
    }

    void setEncoderCount(int i) {
        encoderTicks = (int) i;
    }

    int getEncoderCount() {
        return encoderTicks;
    }


    void risingA() {
        if (digitalRead(encC)) {
            encoderTicks += 1;
        } else {
            encoderTicks -= 1;
        }
    }

    void risingB() {
        if (digitalRead(encA)) {
            encoderTicks += 1;
        } else {
            encoderTicks -= 1;
        } 
    }

    void risingC() {
        if (digitalRead(encB)) {
            encoderTicks += 1;
        } else {
            encoderTicks -= 1;
        } 
    }

    void fallingA() {
        if (digitalRead(encB)) {
            encoderTicks += 1;
        } else {
            encoderTicks -= 1;
        }  
    }

    void fallingB() {
        if (digitalRead(encC)) {
            encoderTicks += 1;
        } else {
            encoderTicks -= 1;
        }  
    }

    void fallingC() {
        if (digitalRead(encA)) {
            encoderTicks += 1;
        } else {
            encoderTicks -= 1;
        }  
    }




    void setupEncoder() {
        pinMode(encA, INPUT);
        pinMode(encB, INPUT);
        pinMode(encC, INPUT);

        attachInterrupt(encA, risingA, RISING);
        attachInterrupt(encB, risingB, RISING);
        attachInterrupt(encC, risingC, RISING);

        attachInterrupt(encA, fallingA, FALLING);
        attachInterrupt(encB, fallingB, FALLING);
        attachInterrupt(encC, fallingC, FALLING);

        // Serial.printf("done setting up encoder. ticks: %d, prevEncoderState: %hhx\n", encoderTicks, prevEncoderState);

    }



}