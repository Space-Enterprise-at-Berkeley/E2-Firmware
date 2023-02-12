#include <Common.h>
#include <EspComms.h>
#include "ReadPower.h"

#include <Arduino.h>
#include "Ducers.h"

uint8_t LED_0 = 18;
uint8_t LED_1 = 19;
uint8_t LED_2 = 20;
uint8_t LED_3 = 21;
uint8_t LED_4 = 7;
uint8_t LED_5 = 33;
uint8_t LED_6 = 34;
uint8_t LED_7 = 35;

uint32_t print_task() { 
  
  Ducers::print_ptSample();
  return 1000 * 1000;
  
}

uint8_t roll = 0;
uint32_t LED_roll(){
  if (roll == 0){
    digitalWrite(LED_0, HIGH);
    digitalWrite(LED_4, HIGH);
    digitalWrite(LED_3, LOW);
    digitalWrite(LED_7, LOW);
    roll = 1;
  }
  else if (roll == 1){
    digitalWrite(LED_1, HIGH);
    digitalWrite(LED_5, HIGH);
    digitalWrite(LED_0, LOW);
    digitalWrite(LED_4, LOW);
    roll = 2;
  }
  else if (roll == 2){
    digitalWrite(LED_2, HIGH);
    digitalWrite(LED_6, HIGH);
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_5, LOW);
    roll = 3;
  }
  else if (roll == 3){
    digitalWrite(LED_3, HIGH);
    digitalWrite(LED_7, HIGH);
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_6, LOW);
    roll = 0;
    return 400*1000;
  }

  return 200*1000;
}

void initLEDs(){
  pinMode(26, INPUT);
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(LED_6, OUTPUT);
  pinMode(LED_7, OUTPUT);
  digitalWrite(LED_0, LOW);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  digitalWrite(LED_4, LOW);
  digitalWrite(LED_5, LOW);
  digitalWrite(LED_6, LOW);
  digitalWrite(LED_7, LOW);
}

Task taskTable[] = {
  //{task_example, 0, true},

  // Ducers
  {Ducers::task_ptSample, 0, true},
  {Power::task_readSendPower, 0, true},
  //{print_task, 0, true},
  {LED_roll, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  initWire();
  Power::init();
  Ducers::init();
  initLEDs();

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
