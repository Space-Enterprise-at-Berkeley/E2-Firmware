#include "TC.h"

namespace TC {
  Comms::Packet tcPacket = {.id = 2};
  MAX31855* tcs = new MAX31855[8];
  int sendRate = 100 * 1000; // 100ms
  SPIClass *vspi;

  void init() {
    //Serial.println("Initializing TCs...");
    uint8_t chipSelectPins[] = { 16, 17, 18, 19, 20, 21, 26, 33 };
    vspi = new SPIClass(HSPI);
    vspi->begin(36, 37, 4, 5);
    //Serial.println("SPI initialized");
    for (uint8_t i = 0; i < 8; i ++) {
      tcs[i] = MAX31855();
      Serial.print("yeeyy");
      tcs[i].init(vspi, chipSelectPins[i]);
      //Serial.print("TC ");
      //Serial.print(i);
      //Serial.println(" initialized");
    }
    //Serial.println("TCs initialized");
  }



  float sample(uint8_t index) {
    return tcs[index].readCelsius();
  }

  uint32_t task_sampleTCs() {
    tcPacket.len = 0;
    for (uint8_t i = 0; i < 8; i ++) {
      Comms::packetAddFloat(&tcPacket, sample(i));
    }
    Comms::emitPacket(&tcPacket);
    return sendRate;
  }

  void print_sampleTCs(){
    for (uint8_t i = 0; i < 8; i ++) {
      Serial.print(sample(i));
      Serial.print(" ");
    }
    Serial.println();
  }
}