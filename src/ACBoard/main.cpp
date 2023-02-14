#include <Common.h>
#include <EspComms.h>
#include <Arduino.h>

#include "AC.h"
#include "ChannelMonitor.h"
#include "MCP23008.h"
#include <Wire.h>


Task taskTable[] = {
  {AC::actuationDaemon, 0, true},
  {AC::actuatorStatesTask, 0, true},
  {ChannelMonitor::readChannels, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  AC::init();
  ChannelMonitor::init(41, 42, 47, 4, 5);


  while(1) {
    // main loop here to avoid arduino overhead
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      uint32_t ticks = micros(); // current time in microseconds
      if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
        taskTable[i].nexttime = ticks + taskTable[i].taskCall();
      }
    }
    //Comms::processWaitingPackets();
  }
}

void loop() {} // unused