#include <Common.h>
#include <EspComms.h>
#include "HAL.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>


// 0: Ground, 1: Flight
uint8_t vehicleState = 0; 

uint32_t helloWorld() {
    Serial.println("yo");
    return 1000 * 1000;
}

char oregbuffer[sizeof(Comms::Packet)];
uint8_t ocnt = 0;
char fregbuffer[sizeof(Comms::Packet)];
uint8_t fcnt = 0;
uint32_t sampleOReg() {
    ocnt = 0;
    while(OREG_SERIAL.available()) {
        Serial.println("o available");
        oregbuffer[ocnt] = OREG_SERIAL.read();
        if(oregbuffer[ocnt] == '\n') {
            Comms::Packet *packet = (Comms::Packet *)&oregbuffer;
            if(Comms::verifyPacket(packet)) {
                DEBUG("Found ereg packet with ID ");
                DEBUG(packet->id);
                DEBUG('\n');
                Comms::emitPacketToGS(packet);
                //Comms::emitPacket(packet, &RADIO_SERIAL, "\r\n\n", 3);
                break;
            }
        }
        ocnt++;
    }
    return 40 * 1000;
}



Task taskTable[] = {
    {sampleOReg, 0, true},
    {helloWorld, 0, true}
};
#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))


void setup() {
  Serial.begin(921600);
  // setup stuff here
  OREG_SERIAL.begin(115200, SERIAL_8N1, HAL::rx0P, HAL::tx0P);
  FREG_SERIAL.begin(115200, SERIAL_8N1, HAL::rx1P, HAL::tx1P);

  Comms::init(); // takes care of Serial.begin()

  Comms::registerCallback(200, [](Comms::Packet packet, uint8_t id) {
    Serial.println("Got packet!");
    Serial.println((char*)packet.data);
  });

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

void loop() {}
