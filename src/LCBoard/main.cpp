#include <Common.h>
#include <EspComms.h>
#include "ADS.h"
#include "ReadPower.h"

#include <Arduino.h>


uint8_t LED1 = 14;
uint8_t LED2 = 15;
uint8_t LED3 = 16;
uint8_t LED4 = 17;
uint8_t LEDS[4] = {LED1, LED2, LED3, LED4};
int roll = 0;


void initLEDs() {
  for (int i = 0; i < 4; i++) {
    pinMode(LEDS[i], OUTPUT);
    digitalWrite(LEDS[i], LOW);
  }
}

uint32_t LED_roll(){
  if (ADS::unrefreshedSample(roll) < -1000){
    digitalWrite(LEDS[roll], LOW);
  }
  roll = (roll + 1) % 4;
  digitalWrite(LEDS[roll], HIGH);
  return 600 * 1000;
}

Task taskTable[] = {
  {ADS::task_sampleLC, 0, true},
  {ADS::printReadings, 0, true},
  {Power::task_readSendPower, 0, true},
  {LED_roll, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))


void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  ADS::init();
  initWire();
  Power::init();
  initLEDs();

  while(1){
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

void loop() {

} // unused
