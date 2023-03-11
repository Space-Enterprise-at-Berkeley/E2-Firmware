#include "RTD.h"

namespace RTD {
  Comms::Packet rtdPacket = {.id = 20};
  MAX31865 *rtd0;
  int sendRate = 100 * 1000; // 100ms
  SPIClass *vspi;

  void init() {
    Serial.println("Initializing RTDs...");
    vspi = new SPIClass(HSPI);
    // SPI.begin(SCK, MISO, MOSI, SS);
    vspi->begin(20, 19, 48, 36); //(36, 37, 4, 5);
    vspi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    Serial.println("SPI initialized");
    rtd0 = new MAX31865(48, vspi);
    rtd0->begin(MAX31865_3WIRE);
    Serial.println("TCs initialized");
  }



    float sample0() { 
        return rtd0->temperature(100, 430);
    }

  uint32_t task_sampleRTD() {
    Comms::packetAddFloat(&rtdPacket, sample0());
    Comms::emitPacket(&rtdPacket);
    return sendRate;
  }

  void print_sampleRTD(){
    Serial.print(sample0());
    Serial.print(" ");
    Serial.println();
  }
}