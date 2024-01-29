#pragma once

#include <Common.h>
#include <EspComms.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

namespace EREG_Comms {

    #define OREG_SERIAL Serial1 
    #define FREG_SERIAL Serial2

    const uint8_t tx0P = 39;
    const uint8_t rx0P = 38;
    
    const uint8_t tx1P = 37;
    const uint8_t rx1P = 36;

    void init();
    void emitPacket(Comms::Packet *packet, HardwareSerial *serialBus);
    void forwardToOreg(Comms::Packet *packet);
    void forwardToFreg(Comms::Packet *packet);
    uint32_t processAvailableData();
    void registerCallback(uint8_t id, Comms::commFunction function);
}