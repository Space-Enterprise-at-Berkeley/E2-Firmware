#include "EReg.h"

namespace EReg {
    char rs485Buffer[sizeof(Comms::Packet)];
    uint8_t cnt = 0;

    void initEReg() {
        pinMode(HAL::RS485_RECEIVE, OUTPUT);
        pinMode(HAL::RS485_TRANSMIT, OUTPUT);

        digitalWriteFast(HAL::RS485_RECEIVE, LOW);
        digitalWriteFast(HAL::RS485_TRANSMIT, LOW);
    }
}