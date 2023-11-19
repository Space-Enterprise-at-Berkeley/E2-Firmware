#pragma once

#include <EspComms.h>
#include <Common.h>
#include "HAL.h"
#include <Arduino.h>

namespace EReg {
    void initEReg();
    uint32_t sampleEReg();
}