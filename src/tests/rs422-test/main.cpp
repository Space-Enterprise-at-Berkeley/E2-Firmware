#include <Arduino.h>

#include "Common.h"
#include "EspComms.h"
#define RS422_SERIAL Serial1

uint32_t helloWorld() {
    Serial.println("yoyo");
    return 500 * 1000;
}

Comms::Packet p = {.id = 1, .len = 0};
uint32_t sendRS422() {
    p.len = 0;
    Comms::packetAddUint8(&p, 69);
    Comms::packetAddUint8(&p, 69);
    //Comms::emitPacket(&p);
    Comms::finishPacket(&p);

    RS422_SERIAL.write(p.id);
    RS422_SERIAL.write(p.len);
    RS422_SERIAL.write(p.timestamp, 4);
    RS422_SERIAL.write(p.checksum, 2);
    RS422_SERIAL.write(p.data, p.len);
    RS422_SERIAL.write('\n');
    Serial.println("rs422 sent");

    return 500 * 1000;
}

char rs422Buffer[sizeof(Comms::Packet)];
int cnt = 0;
uint32_t readRS422() {
    while (RS422_SERIAL.available()) {
        int c = RS422_SERIAL.read();
        rs422Buffer[cnt] = c;
        //Serial.print(c);


        if(rs422Buffer[cnt] == '\n') { //packet end delimiter
            cnt = 0;
            Comms::Packet *packet = (Comms::Packet *)&rs422Buffer;
            // Serial.println("got packet");
            // uint16_t a = (packet->checksum[0] << 8) + packet->checksum[1];
            // Serial.print(Comms::computePacketChecksum(packet));
            // Serial.print(" ");
            // Serial.println(a);
            if(Comms::verifyPacket(packet)) {
                
                //invoke callback
                Serial.print("Received packet of id: ");
                Serial.println(packet->id);
                //Comms::evokeCallbackFunction(packet, FC);
            }
            break;
        }
        cnt++;
        if (cnt > sizeof(Comms::Packet)) {
            cnt = 0;
        }
    }
    return 10;
}

Task taskTable[] = {
  {helloWorld, 0, false},
  {sendRS422, 0, true},
  {readRS422, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  Serial.begin(115200);

    //change these to your board
  int rxpin = 17;
  int txpin = 18;
  RS422_SERIAL.begin(115200, SERIAL_8N1, rxpin, txpin);

  // setup stuff here
  Comms::init(); // takes care of Serial.begin()

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