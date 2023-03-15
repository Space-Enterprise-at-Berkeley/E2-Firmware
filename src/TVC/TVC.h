#pragma once

#include <Arduino.h>
#include <EspComms.h>
#include <PIDController.h>

namespace TVC {
    void init();
    uint32_t updatePID();
}
