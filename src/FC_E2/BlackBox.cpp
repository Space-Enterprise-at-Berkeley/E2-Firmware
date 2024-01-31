#include "BlackBox.h"

namespace BlackBox {

    uint16_t expectedDeviceID=0xEF40;
    int bb_cs = 37;
    SPIFlash flash(bb_cs, expectedDeviceID);
    uint32_t addr;
    bool enable = false;
    bool allowReplay = true;

    uint32_t raddr = 0;
    Comms::Packet replay = {.id=0};
    uint32_t lastPacketTime = 0;
    uint32_t thisPacketTime = 0;

    void init() {
        pinMode(bb_cs, OUTPUT);
        if (flash.initialize()) {
            Serial.println("Init OK!");
        } else {
            Serial.print("Init FAIL, expectedDeviceID(0x");
            Serial.print(expectedDeviceID, HEX);
            Serial.print(") mismatched the read value: 0x");
            Serial.println(flash.readDeviceId(), HEX);
        }
        // enable = true;
        addr = 0;

        Comms::registerCallback(BB_STARTERASE, startRecordingWithErase);
        Comms::registerCallback(BB_START, startRecordingWithoutErase);
        Comms::registerCallback(BB_STOP, stopRecording);

        //replay mode
        Comms::registerCallback(BB_REPLAY, startReplay);

    }

    void writePacket(Comms::Packet *packet) {
        if (flash.busy()) {
            Serial.println("busy");
            return;
        }
        // Serial.printf("busy: %d, erasing: %d, addr: %d, enable: %d, writing\n", flash.busy(), erasing, addr, enable);
        if (enable && addr < (FLASH_SIZE * 0.99)) {
            uint16_t len = 8 + packet->len;
            flash.writeBytes(addr, packet, len);
            addr += len;
            Serial.println("written");
        }
    }

    bool getData(uint32_t byteAddress, Comms::Packet* packet) {
        // Comms::Packet packet;
        flash.readBytes(byteAddress, packet, sizeof(Comms::Packet));
        return Comms::verifyPacket(packet);
    }

    Comms::Packet getData(uint32_t byteAddress) {
        Comms::Packet packet;
        flash.readBytes(byteAddress, &packet, sizeof(Comms::Packet));
        return packet;
    }

    void startRecordingWithErase(Comms::Packet packet, uint8_t ip) {
        Serial.println("starting chip erase");
        flash.chipErase();
        // erasing = true;
        enable = true;
        addr = 0;
    }

    void stopRecording(Comms::Packet packet, uint8_t ip) {
        Serial.println("Stopping logging");
        enable = false;
    }

    void startRecordingWithoutErase(Comms::Packet packet, uint8_t ip) {
        Serial.println("Starting logging wout erase.");
        enable = true;
    }

    void startReplay(Comms::Packet packet, uint8_t ip) {
        while (1) {
            if (WiFiComms::checkAvailable() || Comms::checkAvailable()){
                return;
            }

            if (!BlackBox::getData(raddr, &replay)) {
                Serial.println("packet fail");
                return;
            }
            thisPacketTime = (replay.timestamp[3] << 24) | (replay.timestamp[2] << 16) | (replay.timestamp[1] << 8) | (replay.timestamp[0]);
            uint32_t del = thisPacketTime - lastPacketTime;
            if (del > 1000) {
                del = 10;
            }
            int delayy = 5;
            if (del > (delayy-1)) {
                del -= delayy;
            } else {
            }
            if (replay.id == 12) {
                del = 0;
            } else {
                lastPacketTime = thisPacketTime;
            }
            Serial.printf("packet good, id %d, delaying %d\n", replay.id, del);

            delay(del);
            raddr += 8 + replay.len;
            Comms::emitPacketToGS(&replay);
            WiFiComms::emitPacketToGS(&replay);
        }
        
    }

    void getAllData() {
        for(int i = 0; i < addr; i++) {
            Serial.write(flash.readByte(addr));
        }
    }

    uint32_t getAddr() {
        return addr;
    }

    Comms::Packet sizePacket = {.id = BB_WRITTEN};

    uint32_t reportStoragePacket() { 
        sizePacket.len = 0;
        Comms::packetAddUint32(&sizePacket, (getAddr() / 1000) + (erasing ? 1 : 0));
        Comms::emitPacketToGS(&sizePacket);
        WiFiComms::emitPacketToGS(&sizePacket);
        Radio::forwardPacket(&sizePacket);
        // writePacket(&sizePacket);
        Serial.println("Reported black box packet");
        return 500*1000;
    }
}