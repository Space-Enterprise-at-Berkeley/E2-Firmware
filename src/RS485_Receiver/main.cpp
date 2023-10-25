#include <Common.h>
#include <TeensyComms.h>

#include <Arduino.h>

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define FUEL_SERIAL Serial3 // TODO: verify this
#define FUEL_REN_PIN 18

char fuelBuffer[sizeof(Comms::Packet)];
int fuelCnt = 0;

void setup() {
    // hardware setup
    Serial.begin(115200);
    FUEL_SERIAL.begin(115200);

    Comms::initComms();
    pinMode(FUEL_REN_PIN, OUTPUT);

    digitalWriteFast(FUEL_REN_PIN, LOW);
}

void loop() {
        while(FUEL_SERIAL.available() && fuelCnt < 256) {
            fuelBuffer[fuelCnt] = FUEL_SERIAL.read();
            if(fuelCnt == 0 && ((uint8_t)fuelBuffer[0] != 21 && (uint8_t) fuelBuffer[0] != 22)) {
                Serial.print((uint8_t)fuelBuffer[0]);
                Serial.print(" ");
                continue;
            }
            if(fuelBuffer[fuelCnt] == '\n') {
                Serial.println("got newline");
                Comms::Packet *packet = (Comms::Packet *)&fuelBuffer;
                if(Comms::verifyPacket(packet)) {
                    fuelCnt = 0;
                    Serial.print("fuel: ");
                    Serial.print(packet->id);
                    Serial.print(" : ");
                    Serial.print(Comms::packetGetFloat(packet, 0));
                    Serial.print("\n");
                    Comms::emitPacket(packet);
                    break;
                }
            }
            fuelCnt++;
        }
        if(fuelCnt == 256) {
            Serial.println("RESETTING FUEL BUFFER\n");
            fuelCnt = 0;
        }
        delay(200);
    }