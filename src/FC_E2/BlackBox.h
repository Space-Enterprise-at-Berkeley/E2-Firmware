#pragma once

#include <Arduino.h>
#include <SPIFlash.h>
#include <WiFiCommsLite.h>
#include "Radio.h"
#include "Ethernet.h"

namespace BlackBox {

    const uint32_t FLASH_SIZE = 1.6e7;
    extern bool erasing;

    void init();
    void writePacket(Comms::Packet *packet);
    bool getData(uint32_t byteAddress, Comms::Packet* packet);
    Comms::Packet getData(uint32_t byteAddress);
    void startRecordingWithErase(Comms::Packet packet, uint8_t ip);
    void startRecordingWithoutErase(Comms::Packet packet, uint8_t ip);
    void stopRecording(Comms::Packet packet, uint8_t ip);
    void startReplay(Comms::Packet packet, uint8_t ip);

    // void packetHandler(Comms::Packet packet);

    uint32_t getAddr();
    uint32_t reportStoragePacket();
}