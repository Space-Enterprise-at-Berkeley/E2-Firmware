#include "TC.h"

namespace TC {
  Comms::Packet tcPacket = {.id = 2};
  MAX31855* tcs = new MAX31855[8];

  void init() {
    uint8_t chipSelectPins[] = { 16, 17, 18, 19, 20, 21, 26, 33 };
    vspi = new SPIClass(VSPI);
    vspi->begin(36, 37, 4, 5);
    for (uint8_t i = 0; i < 8; i ++) {
      tcs[0] = MAX31855();
      tcs[0].init(vspi, chipSelectPins[i]);
    }
  }

  float sample(uint8_t index) {
    return tcs[index].readCelsius();
  }

  uint32_t tcSampleTask() {
    tcPacket.len = 0;
    for (uint8_t i = 0; i < 8; i ++) {
      Comms::packetAddFloat(&tcPacket, sample(i));
    }
    Comms::emitPacket(&tcPacket);
    return 100 * 1000;
  }
}