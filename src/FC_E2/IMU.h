#pragma once
// Reads all the accelerometers and barometers on E2


#include <Common.h>
#include <EspComms.h>
#include <Arduino.h>

namespace IMU {
    void init();
    uint32_t task_barometers();
    uint32_t task_accels();
};