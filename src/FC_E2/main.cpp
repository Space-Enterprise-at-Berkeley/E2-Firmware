

#include <Common.h>
#include <EspComms.h>
#include <Wire.h>
#include <Arduino.h>

#include "WiFiCommsLite.h"
#include "Ducers.h"
#include "FlightSensors.h"
#include "Actuators.h"
#include "ChannelMonitor.h"
#include "Automation.h"
#include "ReadPower.h"
#include "BlackBox.h"
#include "Radio.h"
#include "EReg.h"


uint32_t print_task() { 
  Ducers::print_ptSample();
  return 1000 * 1000;
}

int nps;
uint32_t prevTime;

Task taskTable[] = {
  // Ducers

  {Ducers::task_ptSample, 0, true},
  {print_task, 0, true},
  {FlightSensors::task_fancy_barometer, 0, true},
  {FlightSensors::task_barometers, 0, true},
  {FlightSensors::task_accels, 0, true},
  {FlightSensors::task_GPS, 0, true},
  {ChannelMonitor::task_readChannels, 0, true},
  {AC::task_actuationDaemon, 0, true},
  {AC::task_actuatorStates, 0, true},
  {Automation::task_sendAutoventConfig, 0, true},
  {Power::task_readSendPower, 0, true},

  // {WiFiComms::task_WiFiDaemon, 0, true}
  //automation config
  //launch daemon?
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  nps = 0;
  prevTime = millis();

  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);

  Power::init();
  Comms::init(); // takes care of Serial.begin()
  EREG_Comms::init();
  //WiFiComms::init();
  initWire();
  Ducers::init();
  AC::init();
  FlightSensors::init();
  ChannelMonitor::init(7, 6, 5, 3, 4);
  Automation::init();
  //BlackBox::init();
  Radio::initRadio();

  while(1) {
    // main loop here to avoid arduino overhead
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      uint32_t ticks = micros(); // current time in microseconds
      if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
        uint32_t nextTime = taskTable[i].taskCall();
        if (nextTime == 0){
          taskTable[i].enabled = false;
        }
        else {
        taskTable[i].nexttime = ticks + taskTable[i].taskCall();
        }
      }
    }
    Comms::processWaitingPackets();
    EREG_Comms::processAvailableData();
    // WiFiComms::processWaitingPackets();
    Radio::processRadio();

    // Serial.printf("Radio state: %d\n", Radio::transmitting ? 1 : 0);

    if (millis() - prevTime > 1000) {
      prevTime = millis();
      Serial.printf("loops: %d\n", nps);
      nps = 0;
    } else {
      nps ++;
    }
  }
}

void loop() {} // unused