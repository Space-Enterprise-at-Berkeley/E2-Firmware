#include <Common.h>
#include <EspComms.h>
#include "ReadPower.h"

#include <Arduino.h>

#include "TC.h"

uint8_t LED_0 = 34;
uint8_t LED_1 = 38;
uint8_t LED_2 = 39;
uint8_t LED_3 = 40;
uint8_t LED_4 = 41;
uint8_t LED_5 = 42;
uint8_t LED_6 = 45;
uint8_t LED_7 = 46;
uint8_t LEDS[8] = {LED_0, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7};
uint8_t roll = 0;

void initLEDs() {
  for (uint8_t i = 0; i < 8; i ++) {
    pinMode(LEDS[i], OUTPUT);
    digitalWrite(LEDS[i], LOW);
  }
}

uint32_t LED_roll() {
  if (isnan(TC::sample(roll))) {
    digitalWrite(LEDS[roll], LOW);
  }
  roll = (roll + 1) % 8;
  digitalWrite(LEDS[roll], HIGH);
  return 100 * 1000;
}

uint32_t print_task() {
  TC::print_sampleTCs();
  return 1000 * 1000;
}

uint32_t hello_packet() {
  Comms::Packet hello = {.id = 1};
  Comms::packetAddFloat(&hello, 69);
  Comms::packetAddFloat(&hello, 69);
  Comms::packetAddFloat(&hello, 69);
  Comms::emitPacket(&hello);
  return 1000 * 1000;
}

Task taskTable[] = {
  {Power::task_readSendPower,0,true},
  {LED_roll, 0, true},
  //{hello_packet, 0, true},
  {TC::task_sampleTCs, 0, true},
  {print_task, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  initWire();
  initLEDs();
  Power::init();
  TC::init();

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
