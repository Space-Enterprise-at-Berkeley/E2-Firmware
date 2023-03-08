#include <Arduino.h>
#include <Common.h>
#include <TeensyComms.h>
#include "TVC.h"
#include "HAL.h"


uint32_t task_example() { 

  Serial.println("TEST");  
  return 1000 * 1000;
}

Task taskTable[] = {
  // {task_example, 0, true},
  // {TVC::updatePID, 0, true},
  {HAL::printEncoder, 0, true }
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
  // setup stuff here
  Serial.begin(115200);
  #ifdef DEBUG_MODE
  while(!Serial);
  #endif
  Comms::init();
  TVC::init();
  HAL::init();
  HAL::initializeMotorDriver();
  HAL::setEncoderCount(0);
  HAL::setupEncoder();

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
  return 0;
}
