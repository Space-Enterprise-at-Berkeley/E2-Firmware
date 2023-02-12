#pragma once

#include "MAX31855.h"
#include "EspComms.h"
#include <SPI.h>

#include <Arduino.h>

namespace TC {
  //extern SPIClass* vspi;

  void init();

  float sample(uint8_t index);

  uint32_t task_sampleTCs();
  void print_sampleTCs();
}