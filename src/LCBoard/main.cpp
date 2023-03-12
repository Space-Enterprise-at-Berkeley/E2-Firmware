#include <Common.h>
#include <EspComms.h>
#include "ADS.h"
#include "ReadPower.h"

#include <Arduino.h>
// #include <Comms.h>

uint32_t task_example() { 
  Serial.println("Hello World");
  return 1000 * 1000; 
}


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
  if (ADS::unrefreshedSample(roll) < 0){
    digitalWrite(LEDS[roll], LOW);
  }
  roll = (roll + 1) % 4;
  digitalWrite(LEDS[roll], HIGH);
  return 600 * 1000;
}

uint32_t abortDaemon();

Task taskTable[] = {
  {abortDaemon, 0, false},
  {ADS::task_sampleLC, 0, true},
  {ADS::printReadings, 0, true},
  {Power::task_readSendPower, 0, true},
  {LED_roll, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

// Load Cell Abort
// Starts check 2 seconds after recieving flow start packet
// Stops check after recieving abort or end flow packet
// Triggers abort if any load cell is over -100 from flow start weight for 0.5 seconds
uint32_t minThrust = 100;
uint32_t abortTime = 500;
uint32_t abortStartDelay = 2000;
uint32_t timeSinceBad = 0;
float flowStartWeight[4] = {0, 0, 0, 0};


uint32_t abortDaemon(){
  //check if sum less than min thrust from flow start weight for 0.5 seconds
  float sum = 0;
  for (int i = 0; i < 4; i++){
    sum += ADS::unrefreshedSample(i) - flowStartWeight[i];
  }
  if (sum < minThrust){
    if (timeSinceBad == 0){
      timeSinceBad = millis();
    }
    if(millis() - timeSinceBad > abortTime){
      Comms::sendAbort(HOTFIRE, LC_UNDERTHRUST);
      return 0;
    }
  } else {
    timeSinceBad = 0;
  }
  return 75*1000;
}

void onFlowStart(Comms::Packet packet, uint8_t ip) {
  Mode systemMode = (Mode)Comms::packetGetUint8(&packet, 0);
  if (systemMode != HOTFIRE) {
    return;
  }
  //record flow weights
  for (int i = 0; i < 4; i++){
    flowStartWeight[i] = ADS::unrefreshedSample(i);
  }
  //start LC abort daemon when hotfire starts
  taskTable[0].enabled = true;
  taskTable[0].nexttime = micros() + abortStartDelay * 1000;
}

void onAbortOrEndFlow(Comms::Packet packet, uint8_t ip){
  //stop LC abort daemon when abort or endflow is received
  taskTable[0].enabled = false;
}



void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  ADS::init();
  initWire();
  Power::init();
  initLEDs();
  Comms::registerCallback(STARTFLOW, onFlowStart);
  Comms::registerCallback(ABORT, onAbortOrEndFlow);
  Comms::registerCallback(ENDFLOW, onAbortOrEndFlow);


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
