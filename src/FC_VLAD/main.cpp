#include <Common.h>
#include <EspComms.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

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
    {sendState, 0, true},
};
#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
    // hardware setup
    Serial.begin(115200);

    while(1) {
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
            uint32_t ticks = micros(); // current time in microseconds
            if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
                taskTable[i].nexttime = ticks + taskTable[i].taskCall();
            }
        }
        Comms::processWaitingPackets();
    }
}

void loop(){}
