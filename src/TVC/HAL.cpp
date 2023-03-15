#include "HAL.h"

namespace HAL {

    volatile int encoderTicks = 0;

    int32_t numReads = 0;


    int init() {

        setupEncoder();

        pinMode(x_pwm, OUTPUT); 
        pinMode(y_pwm, OUTPUT);

        // ledcSetup(motorChannel, pwmFreq, pwmResolution);
        // ledcAttachPin(SPARKMAX, 0);
        // ledcWrite(motorChannel, 307);

        ledcSetup(0, 50, 12);
        ledcAttachPin(x_pwm, 0);
        ledcWrite(0, 307);

        ledcSetup(1, 50, 12);
        ledcAttachPin(y_pwm, 1);
        ledcWrite(1, 307);

        return 0;
    }

    uint32_t printEncoder() {        
        Serial.println(getEncoderCount());
        return 50 * 1000;
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