#pragma once

#include "EspComms.h"
#include "Ereg.h"
#include "Actuators.h"
#include "EEPROM.h"
#include "WiFiCommsLite.h"
#include "Radio.h"

namespace Automation {

    enum Actuators {
        //AC1
        N2_FLOW = 0,
        LOX_GEMS = 1,
        FUEL_GEMS = 2,
        LIVE_VIDEO = 3,
    };

    enum VehicleState {
        IDLE = 0,
        FLOW = 1,
        FLIGHT = 2,
        REPLAY = 3,
    };

    extern VehicleState vehicleState;

    void init();
    uint32_t task_sendAutoventConfig();
}