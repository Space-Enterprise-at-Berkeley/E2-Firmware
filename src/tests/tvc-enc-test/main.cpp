#include <Arduino.h>
// #include "Util.h"
#define X_PWM_PIN 39

volatile int encoderTicks = 0;

int32_t numReads = 0;
int encA = 19; //encoder A orange
int encB = 18; //encoder B grey
int encC = 21; //encoder C yellow
int x = -50;

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

void runMotor(int32_t speed) {
    if (speed >= -100 && speed <= 50) {
        // analogWrite(X_PWM_PIN, speed + 307);
        ledcWrite(0, speed + 307);
    }
    else {
        // analogWrite(X_PWM_PIN, 307);
        ledcWrite(0, 307);
    }
}

void initMotor() {
    setupEncoder();
    pinMode(X_PWM_PIN, OUTPUT);   
    // analogWriteFrequency(X_PWM_PIN, 50);
    // analogWriteResolution(12);  
    // analogWrite(X_PWM_PIN, 307);
    ledcSetup(0, 50, 12);
    ledcAttachPin(X_PWM_PIN, 0);
    ledcWrite(0, 307);
}

void setup() {
    Serial.begin(115200);
    initMotor(); 
    delay(3000); 
}

void loop() {
    Serial.println(encoderTicks);
    // Serial.println(digitalRead(encA));
    // Serial.println(digitalRead(encB));
    // Serial.println(digitalRead(encC));
    // analogWrite(X_PWM_PIN, 320);
    runMotor(x);
    x += 1;
    delay(100);
}

