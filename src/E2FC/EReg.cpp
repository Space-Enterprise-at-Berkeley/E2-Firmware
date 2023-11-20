#include "EReg.h"

namespace EReg {
    char rs485ReceiveBuffer[sizeof(Comms::Packet)];
    char rs485TransmitBuffer[sizeof(Comms::Packet)];
    uint8_t cnt = 0;
    uint8_t indicatorDuty = 500;
    uint8_t indicatorPeriod = 1000;
    uint8_t indicatorLastTime = 0;

    const int timeBetweenTransmission = 100; // ms
    int lastTransmissionTime = 0;

    const long interval = 25;

    void initEReg() {
        RS485_SERIAL.setPins(HAL::RS485_RECEIVE1, HAL::RS485_TRANSMIT1);
    }

    uint32_t sampleEReg() {
        cnt = 0;
        while (RS485_SERIAL.available()) {
            rs485ReceiveBuffer[cnt] = RS485_SERIAL.read();
            RS485_SERIAL.write(rs485TransmitBuffer, cnt);
            if(rs485ReceiveBuffer[cnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&rs485TransmitBuffer;
                if(Comms::verifyPacket(packet)) {
                    DEBUG("Found ereg packet with ID ");
                    DEBUG(packet->id);
                    DEBUG('\n');
                    Comms::emitPacket(packet);
                    if (packet->id == LOX_EREG) {
                        Comms::emitPacket(packet, LOX_EREG);
                    }
                    break;
                }
            }
            cnt++;
        }
        return 40 * 1000;
    }
}