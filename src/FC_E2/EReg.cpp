#include "EReg.h"

namespace EREG_Comms {

    char oregbuffer[sizeof(Comms::Packet)];
    uint8_t ocnt = 0;
    char fregbuffer[sizeof(Comms::Packet)];
    uint8_t fcnt = 0;
    //so you can callback from packets sent to FC from ereg (gems autovent, abort)
    //std::map<uint8_t, Comms::commFunction> callbackMap; 

    // void forwardToOreg(Comms::Packet packet, int16_t offset) {
    //     //Serial.println("Oreg send");
    //     packet.id += offset;
    //     emitPacket(&packet, &OREG_SERIAL);
    // } 
    // void forwardToOreg(Comms::Packet packet) {
    //     forwardToOreg(packet, 0);
    // } 

    // void forwardToFreg(Comms::Packet packet, int16_t offset) {
    //     //Serial.println("Freg send");
    //     packet.id += offset;
    //     emitPacket(&packet, &FREG_SERIAL);
    // }
    // void forwardToFreg(Comms::Packet packet) {
    //     forwardToFreg(packet, 0);
    // } 

    void forwardToGS(Comms::Packet *packet) {
        // if (packet->id == 133) {
        //     Comms::sendAbort(packet->data[0], packet->data[1]);
        //     return;
        // }
        // else if (packet->id <= 10) {
        //     packet->id += 30;
        //     //Comms::finishPacket(packet); done in emit
        // }
        // else if (packet->id == 102) {
        //     packet->id = 42;
        //     //Comms::finishPacket(packet); done in emit
        // }
        // else if (packet->id == 171) {
        // }
        //Comms::emitPacketToGS(packet);
        //WiFiComms::emitPacketToGS(packet);
        Comms::finishPacket(packet);
        Radio::forwardPacket(packet);
    }

    void emitPacket(Comms::Packet *packet, HardwareSerial *serialBus) {
        Comms::finishPacket(packet);
        // Send over serial
        serialBus->write(packet->id);
        serialBus->write(packet->len);
        serialBus->write(packet->timestamp, 4);
        serialBus->write(packet->checksum, 2);
        serialBus->write(packet->data, packet->len);
        serialBus->write('\n');
    }

    uint32_t processAvailableData() {

        while(FREG_SERIAL.available()) {
            // Serial.println("f available");
            // uint8_t c = FREG_SERIAL.read();
            fregbuffer[fcnt] = FREG_SERIAL.read();
            //Serial.println(c);
            //fregbuffer[fcnt] = FREG_SERIAL.read();
            if(fregbuffer[fcnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&fregbuffer;
                //Serial.println("Got freg packet");
                if(Comms::verifyPacket(packet)) {
                    Serial.print("Found freg packet with ID ");
                    Serial.print(packet->id);
                    Serial.print('\n');
                    
                    forwardToGS(packet);
                    // if (callbackMap.count(packet->id)) //after so offset is applied
                    // {
                    //     callbackMap.at(packet->id)(*packet, FC);
                    // // }
                    // Comms::emitPacket(packet, &RADIO_SERIAL, "\r\n\n", 3);
                }
                fcnt = 0;
                continue;
            }
            fcnt++;
            if (fcnt > sizeof(Comms::Packet)) {
                fcnt = 0;
            }
        }

        while(OREG_SERIAL.available()) {
            // Serial.println("o available");
            // uint8_t c = OREG_SERIAL.read();
            // oregbuffer[ocnt] = c;
            oregbuffer[ocnt] = OREG_SERIAL.read();
            if(oregbuffer[ocnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&oregbuffer;
                //Serial.println("Got oreg packet");
                if(Comms::verifyPacket(packet)) {
                    Serial.print("Found oreg packet with ID ");
                    Serial.print(packet->id);
                    Serial.print('\n');
                    
                    forwardToGS(packet);
                    // if (callbackMap.count(packet->id)) //after so offset is applied
                    // {
                    //     callbackMap.at(packet->id)(*packet, FC);
                    // }
                    //handle packet (forward to GS)
                    // Comms::emitPacket(packet, &RADIO_SERIAL, "\r\n\n", 3);
                }
                ocnt = 0;
                continue;
            }
            ocnt++;
            if (ocnt > sizeof(Comms::Packet)) {
                ocnt = 0;
            }
        }

        return 10;
    }

    // void registerCallback(uint8_t id, Comms::commFunction function)
    // {
    //     callbackMap.insert(std::pair<int, Comms::commFunction>(id, function));
    // }

    void init() {
        OREG_SERIAL.begin(921600, SERIAL_8N1, rx0P, tx0P);
        FREG_SERIAL.begin(921600, SERIAL_8N1, rx1P, tx1P);

        //register callbacks to automatically forward GS -> EREG
        // Comms::registerCallback(200, [](Comms::Packet packet, uint8_t id) {
        //     Serial.println("Got packet!");
        //     Serial.println((char*)packet.data);
        // });

        // Disable forwarding ethernet comms to ereg over RS422 
        // for (int i = 200; i <= 210; i++) {
        //     Comms::registerCallback(i, [](Comms::Packet packet, uint8_t id) {
        //     forwardToOreg(packet, 0);
        //     });
        // }
        // for (int i = 211; i <= 214; i++) {
        //     Comms::registerCallback(i, [](Comms::Packet packet, uint8_t id) {
        //     forwardToOreg(packet, -111);
        //     });
        // }

        // for (int i = 220; i <= 230; i ++) {
        //     Comms::registerCallback(i, [](Comms::Packet packet, uint8_t id) {
        //     forwardToFreg(packet, -20);
        //     });
        // }
        // for (int i = 231; i <= 234; i++) {
        //     Comms::registerCallback(i, [](Comms::Packet packet, uint8_t id) {
        //     forwardToOreg(packet, -131);
        //     });
        // }
        
        // Will likely break when merged, this stuff just needs to get run whenever there's an abort
        // Comms::registerCallback(ABORT, [](Comms::Packet packet, uint8_t id) {
        //     sendToOreg(packet, 0);
        //     sendToFreg(packet, 0);
        // });
    }
}