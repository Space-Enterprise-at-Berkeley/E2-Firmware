#include "RTD.h"

namespace RTD {
  Comms::Packet rtdPacket = {.id = 20};

  int sendRate = 100 * 1000; // 100ms

  void init() {
    // Serial.println("Initializing RTDs...");

    rtd0.begin(MAX31865_3WIRE);
    rtd1.begin(MAX31865_3WIRE);

    // Serial.println("RTD initialized");
  }

  float sample0() { 
    return rtd0.temperature(100, 430);
  }

  float sample1() { 
      return rtd1.temperature(100, 430);
  }

  uint32_t task_sampleRTD() {
    rtdPacket.len = 0;
    Comms::packetAddFloat(&rtdPacket, sample0());
    Comms::packetAddFloat(&rtdPacket, sample1());
    Comms::emitPacket(&rtdPacket);
    return sendRate;
  }

  void print_sampleRTD(){
    Serial.print(sample0());
    Serial.print(sample1());
    Serial.print(" ");
    Serial.println();
  }
}