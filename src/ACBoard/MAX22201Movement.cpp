#include "MAX22201Movement.h";

namespace MAX22201Movement {

uint8_t pin1, pin2;

void init (uint8_t pin1, uint8_t pin2){
    pin1 = pin1;
    pin2 = pin2;

    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
}

void forwards() {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
}

void backwards() {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
}

void stop() {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
}

}
