#include <Common.h>
#include <EspComms.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "FlightStatus.h"
#include "ReadPower.h"
#include "BlackBox.h"
#include "ChannelMonitor.h"

// 0: IDLE, 1: FLIGHT, 2: REPLAY
enum BoardMode {
    IDLE = 0,
    FLIGHT = 1,
    REPLAY = 2,
};
BoardMode mode = IDLE;

// Flight/Launch mode enable
uint8_t setVehicleMode(Comms::Packet statePacket, uint8_t ip){      
    Serial.println("Setting mode to ");           
    mode = (BoardMode) Comms::packetGetUint8(&statePacket, 0);

    // Setup for apogee
    if (mode == FLIGHT) { 
        //Barometer::zeroAltitude();
        //Apogee::start(); 
        // start bb recording  
    } 
    return mode;
}

uint32_t sendState(){
    Comms::Packet state = {.id = 1};
    Comms::packetAddUint8(&state, mode);
    Comms::emitPacketToGS(&state);
    return 1000;
}

uint8_t heartCounter = 0;
Comms::Packet heart = {.id = HEARTBEAT, .len = 0};
void heartbeat(Comms::Packet p, uint8_t ip){
  uint8_t id = Comms::packetGetUint8(&p, 0);
  if (id != ip){
    Serial.println("Heartbeat ID mismatch of " + String(ip) + " and " + String(id));
    return;
  }
  uint8_t recievedCounter = Comms::packetGetUint8(&p, 1);
  if (heartCounter != recievedCounter){
    Serial.println(String(recievedCounter-heartCounter) + " packets dropped");
  }
  Serial.println("Ping from " + String(id) + " with counter " + String(recievedCounter));
  heartCounter = recievedCounter;

  //send it back
  heart.len = 0;
  Comms::packetAddUint8(&heart, IPADDR);
  Comms::packetAddUint8(&heart, heartCounter);
  Comms::emitPacketToGS(&heart);
}

//VLAD needs to:
// Read both barometers
// Read both IMUs
// Read GPS
// Run the Kalman filter/apogee detection
// Read two breakwires
// Send all data over radio
// Record to blackbox chip

//Command Runcams
//Replay blackbox data

Task taskTable[] = {
  // {Power::task_readSendPower, 0, true},
  {FlightStatus::updateFlight, 0, true},
  {Power::task_readSendPower, 0, true},
  {ChannelMonitor::readChannels, 0, true},
  {sendState, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  initWire();
  Power::init();
  FlightStatus::init();
  BlackBox::init();
  ChannelMonitor::init(40, 39, 38, 15, 14);
  Comms::registerCallback(HEARTBEAT, heartbeat);

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

void loop() {

} // unused
