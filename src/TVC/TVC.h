#pragma once

#include <Arduino.h>

#include <PIDController.h>

namespace TVC {
    void init();
    uint32_t updatePID();
}
