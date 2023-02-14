#pragma once

#include <Arduino.h>

#ifdef DEBUG_MODE
#define DEBUG(val) Serial.print(val)
#define DEBUG_FLUSH() Serial.flush()
#define DEBUGLN(val) Serial.println(val)
#define DEBUGF Serial.printf
#else
#define DEBUG(val)
#define DEBUG_FLUSH()
#define DEBUGLN(val)
#define DEBUGF(...)
#endif

struct Task
{
  uint32_t (*taskCall)(void);
  uint32_t nexttime;
  bool enabled;
};

#define initWire() Wire.setClock(400000); Wire.setPins(1,2); Wire.begin()

