#pragma once

#include <Arduino.h>
#include <SPIFlash.h>
#include <WiFiComms.h>
// #include "Radio.h"

namespace BlackBox {

    const uint32_t FLASH_SIZE = 1.6e7;
    extern bool erasing;

    void init();
    void writePacket(Comms::Packet *packet);
    bool getData(uint32_t byteAddress, Comms::Packet* packet);
    Comms::Packet getData(uint32_t byteAddress);
    void startEraseAndRecord();

    // void packetHandler(Comms::Packet packet);

    uint32_t getAddr();
    uint32_t reportStoragePacket();
}