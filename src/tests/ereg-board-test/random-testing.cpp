#include <Arduino.h>

int miso = 36;
int interrupt = 35;
int inhc = 3;
void setup() {
    Serial.begin(115200);
    Serial.printf("hi\n");
    pinMode(interrupt, OUTPUT);
    // pinMode(4, OUTPUT);
    pinMode(miso, OUTPUT);
}

void loop() {
    digitalWrite(interrupt, HIGH);
    digitalWrite(miso, HIGH);
    delay(1000);
    digitalWrite(interrupt, LOW);
    digitalWrite(miso, LOW);
    delay(1000);
}