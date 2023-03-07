#include <Common.h>
#include <EspComms.h>
#include <Arduino.h>
#include "ReadPower.h"

#include "AC.h"
#include "ChannelMonitor.h"
#include <MCP23008.h>
#include <Wire.h>

//Actuators
enum Actuators {
  //AC1
  N2_FLOW = 0,
  ARM = 3,
  LOX_MAIN_VALVE = 4,
  FUEL_MAIN_VALVE = 5,

  //AC2
  N2_FILL = 0,
  N2_VENT = 1,
  IGNITER = 2,

  LOX_VENT_RBV = 4,
  FUEL_VENT_RBV = 5,
  LOX_GEMS = 6,
  FUEL_GEMS = 7,
};

uint32_t loxDelayTime = 500; //500 ms
uint32_t closeLox(); //for delayed abort close lox

Task taskTable[] = {
  {closeLox, 0, false}, // keep this first, only enabled with abort
  {AC::actuationDaemon, 0, true},
  {AC::actuatorStatesTask, 0, true},
  {ChannelMonitor::readChannels, 0, true},
  {Power::task_readSendPower, 0, true},
  {AC::printActuatorStatesTask, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

// ABORT behaviour - 
// switch(abort_reason)
// case TANK OVERPRESSURE:
//    1. Open LOX and FUEL GEMS  
//    2. Open LOX and FUEL Vent RBVs
//    3. Leave Main Valves in current state
// case ENGINE OVERTEMP: 
//    1. Open LOX and FUEL GEMS
//    2. ARM and close LOX and FUEL Main Valves 
// case LC UNDERTHRUST:
//    1. Open LOX and FUEL GEMS
//    2. ARM and close LOX and FUEL Main Valves, fuel first by 0.5 secs to reduce flame.
// case MANUAL/DASHBOARD ABORT:
//    1. Open LOX and FUEL GEMS
//    2. ARM and close LOX and FUEL Main Valves


void onAbort(Comms::Packet packet, uint8_t ip) {
  Mode systemMode = (Mode)packetGetUint8(&packet, 0);
  AbortReason abortReason = (AbortReason)packetGetUint8(&packet, 1);

  switch(abortReason) {
    case TANK_OVERPRESSURE:
      if(ID == AC1){
        //leave main valves in current state
      } else if (ID == AC2){
        //open lox and fuel gems
        AC::actuate(LOX_GEMS, ON, 0);
        AC::actuate(FUEL_GEMS, ON, 0);
        //open lox and fuel vent rbvs
        AC::actuate(LOX_VENT_RBV, RETRACT_FULLY, 0);
        AC::actuate(FUEL_VENT_RBV, RETRACT_FULLY, 0);
      }
      break;
    case ENGINE_OVERTEMP:
      if(ID == AC1){
        //arm and close main valves
        AC::actuate(ARM, ON, 0);
        AC::actuate(LOX_MAIN_VALVE, RETRACT_FULLY, 0);
        AC::actuate(FUEL_MAIN_VALVE, RETRACT_FULLY, 0);
      } else if (ID == AC2){
        //open lox and fuel gems
        AC::actuate(LOX_GEMS, ON, 0);
        AC::actuate(FUEL_GEMS, ON, 0);
      }
      break;
    case LC_UNDERTHRUST:
      if(ID == AC1){
        //arm and close main valves
        AC::actuate(ARM, ON, 0);
        AC::actuate(FUEL_MAIN_VALVE, RETRACT_FULLY, 0);
        //trigger delayed execution of lox valve closure
        taskTable[0].enabled = true;
        taskTable[0].nexttime = micros() + loxDelayTime * 1000;

      } else if (ID == AC2){
        //open lox and fuel gems
        AC::actuate(LOX_GEMS, ON, 0);
        AC::actuate(FUEL_GEMS, ON, 0);
      }    
      break;
    case MANUAL_ABORT:
      if(ID == AC1){
        //arm and close main valves
        AC::actuate(ARM, ON, 0);
        AC::actuate(LOX_MAIN_VALVE, RETRACT_FULLY, 0);
        AC::actuate(FUEL_MAIN_VALVE, RETRACT_FULLY, 0);
      } else if (ID == AC2){
        //open lox and fuel gems
        AC::actuate(LOX_GEMS, ON, 0);
        AC::actuate(FUEL_GEMS, ON, 0);
      }
      break;
  }
}
//only for abort, to allow delayed execution of actuation
uint32_t closeLox(){
  AC::actuate(LOX_MAIN_VALVE, RETRACT_FULLY, 0);
  taskTable[0].enabled = false;
  return 5000*1000; // will never run again, just to satisfy the function signature
}

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  AC::init();
  initWire();
  Power::init();
  ChannelMonitor::init(41, 42, 47, 4, 5);
  //abort register
  Comms::registerCallback(ABORT, onAbort);


  for (int i = 0; i < 8; i++) {
    ChannelMonitor::getMCP1().digitalWrite(i, LOW); 
    ChannelMonitor::getMCP2().digitalWrite(i, LOW); 
  }
      
  for (int i = 0; i < 8; i+=2) {
    ChannelMonitor::getMCP1().digitalWrite(i, HIGH); 
    delay(250);
  }

  for (int i = 0; i < 8; i++) {
    ChannelMonitor::getMCP1().digitalWrite(i, LOW); 
  }

  for (int i = 1; i < 8; i+=2) {
    ChannelMonitor::getMCP1().digitalWrite(i, HIGH); 
    delay(250);
  }

  for (int i = 0; i < 8; i+=2) {
    ChannelMonitor::getMCP2().digitalWrite(i, HIGH); 
    delay(250);
  }

  for (int i = 0; i < 8; i++) {
    ChannelMonitor::getMCP2().digitalWrite(i, LOW); 
  }

  for (int i = 1; i < 8; i+=2) {
    ChannelMonitor::getMCP2().digitalWrite(i, HIGH); 
    delay(250);
  }
 
  

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