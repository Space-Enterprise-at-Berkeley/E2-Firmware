#pragma once

#include "EspComms.h"
#include <Arduino.h>

namespace AC {
  void init();
  float sample(uint8_t index);
  uint32_t actuatorStatesTask();
  uint32_t printActuatorStatesTask();
  uint32_t actuationDaemon();
}