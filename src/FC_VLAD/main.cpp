#include <Common.h>
#include <WiFiComms.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "FlightStatus.h"
#include "ReadPower.h"
#include "BlackBox.h"
#include "ChannelMonitor.h"
#include "Radio.h"
// #include "ReplayFlight.h"

// 0: IDLE, 1: FLIGHT, 2: REPLAY
enum BoardMode {
    IDLE = 0,
    FLIGHT = 1,
    REPLAY = 2,
};
BoardMode mode = FLIGHT; //IDLE;

// Flight/Launch mode enable
uint8_t setVehicleMode(Comms::Packet statePacket, uint8_t ip){      
    Serial.println("Setting mode to ");           
    mode = (BoardMode) Comms::packetGetUint8(&statePacket, 0);

    // Setup for apogee
    if (mode == FLIGHT) { 
        //Barometer::zeroAltitude();
        //Apogee::start(); 
        // start bb recording  
        // BlackBox::startEraseAndRecord();
    } 
    return mode;
}

uint32_t sendState(){
    Comms::Packet state = {.id = 1};
    Comms::packetAddUint8(&state, mode);
    Comms::emitPacketToGS(&state);
    return 1000*1000;
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

int arr[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
int ctr = 0;
void SI446X_CB_SENT(void)
{
    Radio::transmitting = false;
}

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{   
    if(length > MAX_RADIO_TRX_SIZE) length = MAX_RADIO_TRX_SIZE;

    Radio::recvRadio.ready = 1;
    Radio::recvRadio.rssi = rssi;
    Radio::recvRadio.length = length;

    Si446x_read((uint8_t*)Radio::recvRadio.buffer, length);
    Si446x_RX(0);
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	Si446x_RX(0);

	// Printing to serial inside an interrupt is bad!
	// If the serial buffer fills up the program will lock up!
	// Don't do this in your program, this only works here because we're not printing too much data
	Serial.print(F("Packet CRC failed (RSSI: "));
	Serial.print(rssi);
	Serial.println(F(")"));
}

Task taskTable[] = {
  // {Power::task_readSendPower, 0, true},
  {FlightStatus::updateFlight, 0, true},
  {Power::task_readSendPower, 0, true},
  {ChannelMonitor::readChannels, 0, true},
  {BlackBox::reportStoragePacket, 0, true},
  // {sendState, 0, true},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init(); // takes care of Serial.begin()
  initWire();
  Radio::initRadio();
  Power::init();
  FlightStatus::init();
  BlackBox::init();
  BlackBox::startEraseAndRecord();
  ChannelMonitor::init(40, 39, 38, 15, 14);
  // Radio::initRadio();
  Comms::registerCallback(HEARTBEAT, heartbeat);

  if (mode != REPLAY) { 
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
      Si446x_SERVICE();
    }
  }
  else {
    // ReplayFlight::startReplay();
    while(1) {}    
  }

}

void loop() {
} // unused
