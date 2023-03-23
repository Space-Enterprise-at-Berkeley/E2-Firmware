#include "HAL.h"

namespace HAL {

    volatile int encoderTicks_0 = 0;
    volatile int encoderTicks_1 = 0;

    int32_t numReads_0 = 0;
    int32_t numReads_1 = 0;


    int init() {

        setupEncoders();

        pinMode(x_pwm, OUTPUT); 
        pinMode(y_pwm, OUTPUT);

        ledcSetup(0, 50, 12);
        ledcAttachPin(x_pwm, 0);
        ledcWrite(0, 307);

        ledcSetup(1, 50, 12);
        ledcAttachPin(y_pwm, 1);
        ledcWrite(1, 307);
        
        setEncoderCount_0(0);
        setEncoderCount_1(0);

        return 0;
    }

    uint32_t printEncoder_0() {        
        Serial.println(getEncoderCount_0());
        return 50 * 1000;
    }

    uint32_t printEncoder_1() {        
        Serial.println(getEncoderCount_1());
        return 50 * 1000;
    }


    void setEncoderCount_0(int i) {
        encoderTicks_0 = (int) i;
    }

    int getEncoderCount_0() {
        return encoderTicks_0;
    }

    
    void setEncoderCount_1(int i) {
        encoderTicks_1 = (int) i;
    }

    int getEncoderCount_1() {
        return encoderTicks_1;
    }


    void risingA_0() {
        if (digitalRead(encC_0)) {
            encoderTicks_0 += 1;
        } else {
            encoderTicks_0 -= 1;
        }
        
    }

    void risingB_0() {
        if (digitalRead(encA_0)) {
            encoderTicks_0 += 1;
        } else {
            encoderTicks_0 -= 1;
        } 
    }

    void risingC_0() {
        if (digitalRead(encB_0)) {
            encoderTicks_0 += 1;
        } else {
            encoderTicks_0 -= 1;
        } 
    }

    void risingA_1() {
        if (digitalRead(encC_1)) {
            encoderTicks_1 += 1;
        } else {
            encoderTicks_1 -= 1;
        }
        
    }

    void risingB_1() {
        if (digitalRead(encA_1)) {
            encoderTicks_1 += 1;
        } else {
            encoderTicks_1 -= 1;
        } 
    }

    void risingC_1() {
        if (digitalRead(encB_1)) {
            encoderTicks_1 += 1;
        } else {
            encoderTicks_1 -= 1;
        } 
    }


    void fallingA_0() {
        if (digitalRead(encB_0)) {
            encoderTicks_0 += 1;
        } else {
            encoderTicks_0 -= 1;
        }  
    }

    void fallingB_0() {
        if (digitalRead(encC_0)) {
            encoderTicks_0 += 1;
        } else {
            encoderTicks_0 -= 1;
        }  
    }

    void fallingC_0() {
        if (digitalRead(encA_0)) {
            encoderTicks_0 += 1;
        } else {
            encoderTicks_0 -= 1;
        }  
    }

    void fallingA_1() {
        if (digitalRead(encB_1)) {
            encoderTicks_1 += 1;
        } else {
            encoderTicks_1 -= 1;
        }  
    }

    void fallingB_1() {
        if (digitalRead(encC_1)) {
            encoderTicks_1 += 1;
        } else {
            encoderTicks_1 -= 1;
        }  
    }

    void fallingC_1() {
        if (digitalRead(encA_1)) {
            encoderTicks_1 += 1;
        } else {
            encoderTicks_1 -= 1;
        }  
    }


    void changeA_0() {
        if (digitalRead(encA_0)) {
            risingA_0();
        } else {
            fallingA_0();
        }
    }
    
    void changeB_0() {
        if (digitalRead(encB_0)) {
            risingB_0();
        } else {
            fallingB_0();
        }
    }

    void changeC_0() {
        if (digitalRead(encC_0)) {
            risingC_0();
        } else {
            fallingC_0();
        }
    }

    void changeA_1() {
        if (digitalRead(encA_1)) {
            risingA_1();
        } else {
            fallingA_1();
        }
    }
    
    void changeB_1() {
        if (digitalRead(encB_1)) {
            risingB_1();
        } else {
            fallingB_1();
        }
    }

    void changeC_1() {
        if (digitalRead(encC_1)) {
            risingC_1();
        } else {
            fallingC_1();
        }
    }


    void setupEncoders() {
        pinMode(encA_0, INPUT);
        pinMode(encB_0, INPUT);
        pinMode(encC_0, INPUT);

        pinMode(encA_1, INPUT);
        pinMode(encB_1, INPUT);
        pinMode(encC_1, INPUT);

        attachInterrupt(encA_0, changeA_0, CHANGE);
        attachInterrupt(encB_0, changeB_0, CHANGE);
        attachInterrupt(encC_0, changeC_0, CHANGE);

        attachInterrupt(encA_1, changeA_1, CHANGE);
        attachInterrupt(encB_1, changeB_1, CHANGE);
        attachInterrupt(encC_1, changeC_1, CHANGE);
        // Serial.printf("done setting up encoder. ticks: %d, prevEncoderState: %hhx\n", encoderTicks_0, prevEncoderState);
    }

    void resetEncoders() { 
        setEncoderCount_0(0);
        setEncoderCount_1(0);
    }

}