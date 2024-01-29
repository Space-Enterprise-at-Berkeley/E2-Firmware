#include <Common.h>
#include <EspComms.h>
#include "HAL.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

void sendToOreg(Comms::Packet packet, int8_t offset) {
  Serial.println("Oreg send");
  packet.id += offset;
  Comms::finishPacket(&packet);
  OREG_SERIAL.write(packet.id);
  OREG_SERIAL.write(packet.len);
  OREG_SERIAL.write(packet.timestamp, 4);
  OREG_SERIAL.write(packet.checksum, 2);
  OREG_SERIAL.write(packet.data, packet.len);
  OREG_SERIAL.write('\n');
} 

void sendToFreg(Comms::Packet packet, int8_t offset) {
  Serial.println("Freg send");
  packet.id += offset;
  Comms::finishPacket(&packet);
  FREG_SERIAL.write(packet.id);
  FREG_SERIAL.write(packet.len);
  FREG_SERIAL.write(packet.timestamp, 4);
  FREG_SERIAL.write(packet.checksum, 2);
  FREG_SERIAL.write(packet.data, packet.len);
  FREG_SERIAL.write('\n');
}

// 0: Ground, 1: Flight
uint8_t vehicleState = 0; 

uint32_t helloWorld() {
    Comms::Packet packet = {.id = 'E', .len = 0};
    Comms::finishPacket(&packet);
    sendToOreg(packet, 0);
    sendToFreg(packet, 0);
    return 1000 * 1000;
}

char oregbuffer[sizeof(Comms::Packet)];
uint8_t ocnt = 0;
char fregbuffer[sizeof(Comms::Packet)];
uint8_t fcnt = 0;
uint32_t sampleOReg() {
    while(OREG_SERIAL.available()) {
        // Serial.println("o available");
        uint8_t c = OREG_SERIAL.read();
        oregbuffer[ocnt] = c;
        if(oregbuffer[ocnt] == '\n') {
            Comms::Packet *packet = (Comms::Packet *)&oregbuffer;
            Serial.println("Got oreg packet");
            if(Comms::verifyPacket(packet)) {
                Serial.print("Found oreg packet with ID ");
                Serial.print(packet->id);
                Serial.print('\n');
                if (packet->id == 133) {
                  Comms::sendAbort(packet->data[0], packet->data[1]);
                }
                else if (packet->id <= 10) {
                  packet->id += 30;
                  Comms::finishPacket(packet);
                  Comms::emitPacketToGS(packet);
                }
                else if (packet->id == 102) {
                  packet->id = 42;
                  Comms::finishPacket(packet);
                  Comms::emitPacketToGS(packet);
                }
                //Comms::emitPacket(packet, &RADIO_SERIAL, "\r\n\n", 3);
            }
            ocnt = 0;
            break;
        }
        ocnt++;
        if (ocnt > sizeof(Comms::Packet)) {
          ocnt = 0;
        }
    }
    while(FREG_SERIAL.available()) {
        // Serial.println("f available");
        uint8_t c = FREG_SERIAL.read();
        fregbuffer[fcnt] = c;
        if(fregbuffer[fcnt] == '\n') {
            Comms::Packet *packet = (Comms::Packet *)&fregbuffer;
            Serial.println("Got freg packet");
            if(Comms::verifyPacket(packet)) {
                Serial.print("Found freg packet with ID ");
                Serial.print(packet->id);
                Serial.print('\n');
                if (packet->id == 133) {
                  Comms::sendAbort(packet->data[0], packet->data[1]);
                }
                else if (packet->id <= 10) {
                  packet->id += 50;
                  Comms::finishPacket(packet);
                  Comms::emitPacketToGS(packet);
                }
                else if (packet->id == 102) {
                  packet->id = 62;
                  Comms::finishPacket(packet);
                  Comms::emitPacketToGS(packet);
                }
                //Comms::emitPacket(packet, &RADIO_SERIAL, "\r\n\n", 3);
            }
            fcnt = 0;
            break;
        }
        fcnt++;
        if (fcnt > sizeof(Comms::Packet)) {
          fcnt = 0;
        }
    }
    return 10;
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

  for (int i = 200; i <= 210; i ++) {
    Comms::registerCallback(i, [](Comms::Packet packet, uint8_t id) {
      sendToOreg(packet, 0);
    });
  }

  for (int i = 220; i <= 230; i ++) {
    Comms::registerCallback(i, [](Comms::Packet packet, uint8_t id) {
      sendToFreg(packet, -20);
    });
  }
  
  // Will likely break when merged, this stuff just needs to get run whenever there's an abort
  Comms::registerCallback(ABORT, [](Comms::Packet packet, uint8_t id) {
    sendToOreg(packet, 0);
    sendToFreg(packet, 0);
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
