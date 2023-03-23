#include <Arduino.h>
#include <Common.h>
#include <EspComms.h>
#include "TVC.h"
#include "HAL.h"


uint32_t task_example() { 

  Serial.println("TEST");  
  return 1000 * 1000;
}

Task taskTable[] = {
  // {task_example, 0, true},
  {TVC::updatePID, 0, true},
  // {HAL::printEncoder_0, 0, true},
  // {HAL::printEncoder_1, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init();
  // Comms::init(10, );
  // int cs, int spiMisoPin, int spiMosiPin, int spiSclkPin, int ETH_intN
  // Serial.begin(921600);
  HAL::init();
  HAL::setupEncoders();
  HAL::resetEncoders();
  TVC::init();
  Comms::registerCallback(1, TVC::definePosition);
  // Comms::registerCallback(2, TVC::printEncoders);
  // Comms::registerCallback(2, TVC::slowRun);

  while(1) {
    // main loop here to avoid arduino overhead
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      uint32_t ticks = micros(); // current time in microseconds
      if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
        uint32_t delayoftask = taskTable[i].taskCall();
        if (delayoftask == 0) {
          taskTable[i].enabled = false;
        }
        else {
          taskTable[i].nexttime = ticks + delayoftask;
        }
      }
    }
    Comms::processWaitingPackets();
  }
}

void loop() {} // unused

