#pragma once
// Need to add readChannels task from Evan

#include "EspComms.h"
#include <Arduino.h>

namespace AC {

  void init();
  float sample(uint8_t index);
  uint32_t tcSampleTask();
}