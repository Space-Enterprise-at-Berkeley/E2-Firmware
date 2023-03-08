#include "HAL.h"

namespace HAL {

    volatile int encoderTicks = 0;

    int32_t numReads = 0;

    // int openLimitSwitchEngaged = 0;
    // int closedLimitSwitchEngaged = 0;

    bool motorDriverEnabled;


    int init() {

        setupEncoder();

        int pwmFreq = 50;
        int pwmResolution = 12;

        // ledcSetup(motorChannel, pwmFreq, pwmResolution);
        // ledcAttachPin(SPARKMAX, 0);
        // ledcWrite(motorChannel, 307);
// 
        return 0;
    }

    
    int initializeMotorDriver() {
        Serial.printf("motor driver init start\n");

        int pwmFreq = 50000;
        int pwmResolution = 8;
        // ledcSetup(motorChannel, pwmFreq, pwmResolution);
        // ledcAttachPin(INHA, 0);
        // ledcWrite(motorChannel, 0);

        enableMotorDriver();
        delay(10);
        // printMotorDriverFaultAndDisable();
        enableMotorDriver();
        delay(10);

        delay(1);
        return 0;

    }

    uint32_t printEncoder() {        
        Serial.println(getEncoderCount());
        return 50 * 1000;
    }


    void enableMotorDriver() {
        digitalWrite(DRV_EN, HIGH);
        motorDriverEnabled = true;
    }

    void disableMotorDriver() {
        digitalWrite(DRV_EN, LOW);
        motorDriverEnabled = false;
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

    void changeA() {
        if (digitalRead(encA)) {
            risingA();
        } else {
            fallingA();
        }
    }
    
    void changeB() {
        if (digitalRead(encB)) {
            risingB();
        } else {
            fallingB();
        }
    }

    void changeC() {
        if (digitalRead(encC)) {
            risingC();
        } else {
            fallingC();
        }
    }

    void setupEncoder() {
        pinMode(encA, INPUT);
        pinMode(encB, INPUT);
        pinMode(encC, INPUT);

        attachInterrupt(encA, changeA, CHANGE);
        attachInterrupt(encB, changeB, CHANGE);
        attachInterrupt(encC, changeC, CHANGE);
        // Serial.printf("done setting up encoder. ticks: %d, prevEncoderState: %hhx\n", encoderTicks, prevEncoderState);

    }

}