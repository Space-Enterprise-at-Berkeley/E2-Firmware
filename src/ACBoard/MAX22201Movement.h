#pragma once

#include <Arduino.h>

namespace MAX22201Movement{

    uint8_t pin1, pin2;

    void init (uint8_t pin1, uint8_t pin2);

    void forwards();
    void backwards();
    void stop();
}