#pragma once

#include <Arduino.h>
#include <EspComms.h>
#include <PIDController.h>

namespace TVC {
    void init();
    uint32_t updatePID();
    void definePosition(Comms::Packet defPos, uint8_t id);
}
