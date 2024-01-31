#pragma once
// Reads all the accelerometers and barometers on E2


#include <Common.h>
#include <EspComms.h>
#include "WiFiCommsLite.h"
#include "Radio.h"
#include <Arduino.h>
#include "Radio.h"

namespace FlightSensors {
    void init();
    uint32_t task_barometers();
    uint32_t task_accels();
    uint32_t task_GPS();
};