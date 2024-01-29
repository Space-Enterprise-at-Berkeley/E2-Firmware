#pragma once

#include "EspComms.h"
#include <SPI.h>

#include <Arduino.h>
#include "MAX31865.h"

#include "HAL.h"

namespace RTD {

  void init();

  uint32_t task_sampleRTD();

  void print_sampleRTD();
}