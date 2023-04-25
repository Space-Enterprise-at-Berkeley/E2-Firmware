#include "ReplayFlight.h"

namespace ReplayFlight {

    uint32_t addr = 0;
    Comms::Packet packet1 = {.id=150};
    uint32_t lastPacketTime = 0;
    uint32_t thisPacketTime = 0;

    void startReplay() {
        while (1) {
            if (!BlackBox::getData(addr, &packet1)) {
                Serial.println("packet fail");
                return;
            }
            thisPacketTime = (packet1.timestamp[3] << 24) | (packet1.timestamp[2] << 16) | (packet1.timestamp[1] << 8) | (packet1.timestamp[0]);
            uint32_t del = thisPacketTime - lastPacketTime;
            if (del > 1000) {
                del = 10;
            }
            int delayy = 5;
            if (del > (delayy-1)) {
                del -= delayy;
            } else {
            }
            if (packet1.id == 12) {
                del = 0;
            } else {
                lastPacketTime = thisPacketTime;
            }
            Serial.printf("packet good, id %d, delaying %d\n", packet1.id, del);

            delay(del);
            addr += 8 + packet1.len;
            Comms::emitPacketToGS(&packet1);
        }
    }



}