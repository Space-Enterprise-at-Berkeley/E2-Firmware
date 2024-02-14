

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



Task taskTable[] = {
  // Ducers
  {Ducers::task_ptSample, 0, true},
  {print_task, 0, true},
  {FlightSensors::task_fancy_barometer, 0, true},
  {FlightSensors::task_barometers, 0, true},
  {FlightSensors::task_accels, 0, true},
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

TaskHandle_t Task0; //multicore tasks, not our scheduler tasks
TaskHandle_t Task1;


void core0Task(void * parameter) {

  Serial.printf("started on core 0\n");

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
  }
}

void core1Task(void * parameter) {

  Serial.printf("started on core 1\n");

  while (69) {
    Radio::processTransmitStack();
    delay(1);
    Radio::processRadio();
  }


}

void setup() {

  Serial.begin(921600);
  Serial.printf("hi!!\n");

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
  Radio::initRadio();
  //BlackBox::init();
  Serial.printf("init done\n");
  delay(100);

  Serial.printf("sending off core 1\n");
  xTaskCreatePinnedToCore(
                  core1Task,   /* Task function. */
                  "Task1",     /* name of task. */
                  20000,       /* Stack size of task */
                  NULL,        /* parameter of the task */
                  8,           /* priority of the task */
                  &Task1,      /* Task handle to keep track of created task */
                  1);          /* pin task to core 1 */

  delay(10);

  Serial.printf("sending off core 0\n");
  xTaskCreatePinnedToCore(
                  core0Task,   /* Task function. */
                  "Task0",     /* name of task. */
                  20000,       /* Stack size of task */
                  NULL,        /* parameter of the task */
                  8,           /* priority of the task */
                  &Task0,      /* Task handle to keep track of created task */
                  0);          /* pin task to core 0 */




}

void loop() {
  
}