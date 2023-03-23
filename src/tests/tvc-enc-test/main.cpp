#include <Arduino.h>
// #include "Util.h"
// #define X_PWM_PIN 39
#define X_PWM_PIN 15

volatile int encoderTicks = 0;

int32_t numReads = 0;
// int encA = 20; //encoder A orange
// int encB = 18; //encoder B grey
// int encC = 21; //encoder C yellow

// int encA = 34;
// int encB = 21;
// int encC = 20;
    // const int encA_1 = 14;
    // const int encB_1 = 17;
    // const int encC_1 = 18;

int encA = 14;
int encB = 17;
int encC = 18;

int x = -30;
int dec = 1;

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
    } 
    else {
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
    if (speed >= -40 && speed <= 40) {
        ledcWrite(0, speed + 307);
    }
    else {
        ledcWrite(0, 307);
    }
}

void initMotor() {
    setupEncoder();
    pinMode(X_PWM_PIN, OUTPUT);   
    ledcSetup(0, 50, 12);
    ledcAttachPin(X_PWM_PIN, 0);
    ledcWrite(0, 307);
}

void setup() {
    Serial.begin(115200);
    initMotor(); 
    delay(1000); 
}

void loop() {
    Serial.print("Count: ");
    Serial.println(getEncoderCount());
    Serial.print("A: ");
    Serial.println(digitalRead(encA));
    Serial.print("B: ");
    Serial.println(digitalRead(encB));
    Serial.print("C: ");
    Serial.println(digitalRead(encC));
    runMotor(x);
    x += dec;
    delay(100);
    if (x == 40) { 
        dec = -1;
    } 
    if (x == -40) { 
        dec = 1;
    }
}

