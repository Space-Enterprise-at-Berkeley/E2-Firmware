#include <Arduino.h>    

void setup() {
    Serial.begin(115200);
    Serial.println("Hello World");

    const int x_pwm = 39; //sparkmax0
    const int y_pwm = 15; //sparkmax1

    pinMode(x_pwm, OUTPUT); 
    pinMode(y_pwm, OUTPUT);

    //0 - 4096
    // 307 +- 100


    ledcSetup(0, 50, 12);
    ledcAttachPin(x_pwm, 0);
    ledcWrite(0, 307);

    ledcSetup(1, 50, 12);
    ledcAttachPin(y_pwm, 1);
    ledcWrite(1, 307);
}


void loop() {

    Serial.printf("high\n");
    ledcWrite(1, 307 + 5);
    delay(5000);
    ledcWrite(1, 307);
    delay(1000);
    Serial.printf("low\n");
    ledcWrite(1, 307 - 5);
    delay(5000);
    ledcWrite(1, 307);
    delay(1000);


}