#include <Arduino.h>
#define X_PWM_PIN 6

volatile int encoderTicks = 0;

int32_t numReads = 0;
int encA = 27; //encoder A
int encB = 25; //encoder B
int encC = 24; //encoder C

bool motorDriverEnabled;

void setEncoderCount(int i) {
    encoderTicks = (int) i;
}

int getEncoderCount() {
    return encoderTicks;
}

uint32_t printEncoder() {        
    Serial.println(getEncoderCount());
    return 50 * 1000;
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

void setup() {
    Serial.begin(921600);
    while (!Serial);
    setupEncoder();
    pinMode(OUTPUT, X_PWM_PIN);   
    analogWriteFrequency(X_PWM_PIN, 50);
    analogWriteResolution(12);  
    analogWrite(X_PWM_PIN, 307); 
    delay(3000); 
}

void loop() {
    Serial.println(encoderTicks);
    analogWrite(X_PWM_PIN, 320);
    delay(100);
}
