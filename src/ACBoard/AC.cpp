#include "AC.h"
#include "MAX22201.h"



MAX22201 drivers[8];

namespace AC {

  void init() {
    drivers[0] = new MAX22201();
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