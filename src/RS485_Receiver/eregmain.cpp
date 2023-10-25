#include <Common.h>
#include <TeensyComms.h>

#include <Arduino.h>

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define TEST_SERIAL Serial3 // TODO: verify this
#define RECEIVE_PIN 18

char buffer[sizeof(Comms::Packet)];
int cnt = 0;

void setup() {
    // hardware setup
    Serial.begin(115200);
    TEST_SERIAL.begin(115200);

    Comms::initComms();
    pinMode(RECEIVE_PIN, OUTPUT);

    digitalWriteFast(RECEIVE_PIN, LOW);
}

void loop() {
        while(TEST_SERIAL.available() && cnt < 256) {
            Serial.println("AVAILABLEEEE");
            buffer[cnt] = TEST_SERIAL.read();
            if(cnt == 0 && ((uint8_t)buffer[0] != 23)) {
                Serial.print((uint8_t)buffer[0]);
                Serial.print(" ");
                continue;
            }
            if(buffer[cnt] == '\n') {
                Serial.println("got newline");
                Comms::Packet *packet = (Comms::Packet *)&buffer;
                if(Comms::verifyPacket(packet)) {
                    cnt = 0;
                    Serial.print("EREG: ");
                    Serial.print(packet->id);
                    Serial.print(" : ");
                    Serial.print(Comms::packetGetFloat(packet, 0));
                    Serial.print("\n");
                    Comms::emitPacket(packet);
                    break;
                }
            }
            cnt++;
        }
        if(cnt == 256) {
            Serial.println("RESETTING BUFFER\n");
            cnt = 0;
        }
        delay(200);
    }
