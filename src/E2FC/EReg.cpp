#include "EReg.h"

namespace EReg {
    char buffer[sizeof(Comms::Packet)];
    char buffer2[sizeof(Comms::Packet)];
    int cnt = 0;

    void initEReg() {
        digitalWriteFas
    }

}

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define TEST_SERIAL Serial3 // TODO: verify this
#define RECEIVE_PIN 18
#define TRANSMIT_PIN 19

void setup() {
    // hardware setup
    Serial.begin(115200);
    TEST_SERIAL.begin(115200);

    Comms::initComms();
    pinMode(RECEIVE_PIN, OUTPUT);
    pinMode(TRANSMIT_PIN, OUTPUT);

    digitalWriteFast(RECEIVE_PIN, LOW);
    digitalWriteFast(TRANSMIT_PIN, HIGH);

    for (int i = 0; i < sizeof(Comms::Packet); i++) {
        buffer2[i] = 84;
    }
    // TRANSMITS T
}

void loop() {
        while(TEST_SERIAL.available() && cnt < 256) {
            Serial.println("AVAILABLEEEE - teensy side");
            buffer[cnt] = TEST_SERIAL.read();
            Serial.print("printing from ereg --> teensy?: ");
            Serial.println(buffer[cnt]);
            TEST_SERIAL.write(buffer2, cnt);
            // if(cnt == 0 && ((uint8_t)buffer[0] != 23)) {
            //     Serial.print((uint8_t)buffer[0]);
            //     Serial.print(" ");
            //     continue;
            // }
            // if(buffer[cnt] == '\n') {
            //     Serial.println("got newline");
            //     Comms::Packet *packet = (Comms::Packet *)&buffer;
            //     if(Comms::verifyPacket(packet)) {
            //         cnt = 0;
            //         Serial.print("EREG: ");
            //         Serial.print(packet->id);
            //         Serial.print(" : ");
            //         Serial.print(Comms::packetGetFloat(packet, 0));
            //         Serial.print("\n");
            //         Comms::emitPacket(packet);
            //         break;
            //     }
            // }
            cnt++;
        }
        if(cnt == 256) {
            Serial.println("RESETTING BUFFER\n");
            cnt = 0;
        }
        delay(200);
    }