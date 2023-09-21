#include <Arduino.h>
#include <Common.h>
#include <EspComms.h>
#include "TVC.h"
#include "HAL.h"


uint32_t task_example() { 

  Serial.println("TEST");  
  return 1000 * 1000;
}

/* bool joystickActive = false;
void joystickCommand(Comms::Packet joystickCommand, uint8_t ip) {
  // If the joystick is active, then we should be updating position targets from it
  if (joystickActive){
    TVC::definePosition(joystickCommand, ip);
  }
}
 */
Task taskTable[] = {
  {TVC::zero, 0, false}, //disabled by default
  {TVC::moveTVC, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void zeroTVC(Comms::Packet packet, uint8_t ip) { 
  Serial.printf("zeroing...\n");
  if (taskTable[0].enabled) return; 
  taskTable[0].enabled = true;
  taskTable[0].nexttime = micros();
}

void setup() {
  // setup stuff here
  Serial.begin(115200);
  Serial.printf("hii!!\n");
  Comms::init();
  HAL::init();
  HAL::setupEncoders();
  HAL::resetEncoders();
  TVC::init();
  Comms::registerCallback(1, TVC::definePosition);
  Comms::registerCallback(2, TVC::enableCircle);
  Comms::registerCallback(3, zeroTVC);
  Comms::registerCallback(4, TVC::stopTVC);
  Comms::registerCallback(5, TVC::setTVCMode);
  // Comms::registerCallback(101, joystickCommand);
  // Comms::registerCallback(2, TVC::printEncoders);

  Serial.printf("Setup complete\n");

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

