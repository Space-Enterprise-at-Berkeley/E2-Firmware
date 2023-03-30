#pragma once

#include <Arduino.h>
#include <EspComms.h>
#include <PIDController.h>
namespace TVC {

    void init();
    // uint32_t updatePID();
    void definePosition(Comms::Packet defPos, uint8_t id);
    void defineSpeeds(Comms::Packet defSpeeds, uint8_t id);

    uint32_t moveTVC();
    void enableCircle(Comms::Packet statePacket, uint8_t ip); 
    void setMode(int mode);
    void setXSpeed(int spdx);
    void setYSpeed(int spdy);
    void stopTVC();
    uint32_t zero();
}
