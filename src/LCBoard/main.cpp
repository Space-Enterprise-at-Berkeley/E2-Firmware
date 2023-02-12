#include <Common.h>
#include <EspComms.h>
#include "ADS.h"

#include <Arduino.h>

uint32_t refreshADCReadings() { 
  return ADS::sampleLC();
}

uint32_t printReadings(){
  return ADS::printReadings();
}

Task taskTable[] = {
  {refreshADCReadings, 0, true},
  {printReadings, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  ADS::init();

  while(1) {
    // main loop here to avoid arduino overhead
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      uint32_t ticks = micros(); // current time in microseconds
      if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
        taskTable[i].nexttime = ticks + taskTable[i].taskCall();
      }
    }
    Comms::processWaitingPackets();
  }
}

void loop() {} // unused
