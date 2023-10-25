#include <Common.h>
#include <TeensyComms.h>

#include <Arduino.h>

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define TEST_SERIAL Serial3 // TODO: verify this
// #define TRANSMIT_PIN 19
#define RECEIVE_PIN 18

char buffer[sizeof(Comms::Packet)];
int cnt = 0;

void setup() {
    // hardware setup
    Serial.begin(115200);
    Serial.println("SETUP");
    TEST_SERIAL.begin(115200);

    Comms::initComms();

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    // pinMode(TRANSMIT_PIN, OUTPUT);
    pinMode(RECEIVE_PIN, OUTPUT);

    // digitalWriteFast(TRANSMIT_PIN, HIGH);
    digitalWriteFast(RECEIVE_PIN, LOW);
}

void loop() {
    Serial.println("MAIN LOOP");
    while(TEST_SERIAL.available() && cnt < 256) {
        Serial.println("ITS AVAILABLE!!");
        buffer[cnt] = TEST_SERIAL.read();
        if(cnt == 0 && ((uint8_t)buffer[0] != 21 && (uint8_t) buffer[0] != 22)) {
            Serial.print((uint8_t)buffer[0]);
            Serial.print(" ");
            continue;
        }
        if(buffer[cnt] == '\n') {
            Serial.println("got newline");
            Comms::Packet *packet = (Comms::Packet *)&buffer;
            if(Comms::verifyPacket(packet)) {
                cnt = 0;
                Serial.print("fuel: ");
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

// #define E_REG_TEST_TRANSMIT 19
// #define E_REG_TEST_RECEIVE 18

// char loxBuffer[sizeof(Comms::Packet)];
// int loxCnt = 0;

// void setup() {
//     // hardware setup
//     Serial.begin(115200);
//     TEST_SERIAL.begin(115200);

//     Comms::initComms();

//     Serial.println("SETUP");

//     DEBUG("STARTING UP\n");
//     DEBUG_FLUSH();

//     pinMode(E_REG_TEST_TRANSMIT, OUTPUT);
//     pinMode(E_REG_TEST_RECEIVE, OUTPUT);

//     digitalWriteFast(E_REG_TEST_TRANSMIT, LOW);
//     digitalWriteFast(E_REG_TEST_RECEIVE, LOW);
//     delay(200);
// }

// void loop() {
//         Serial.println("loop");
//         while(TEST_SERIAL.available() && loxCnt < 256) {
//             Serial.println("serial available");
//             TEST_SERIAL.write(loxBuffer, loxCnt);
//             loxBuffer[loxCnt] = TEST_SERIAL.read();
//             if(loxBuffer[loxCnt] == '\n') {
//                 Serial.println("got newline");
//             }
//             loxCnt++;
//         }
//         if(loxCnt == 256) {
//             Serial.println("RESETTING LOX BUFFER\n");
//             loxCnt = 0;
//         }
//         delay(200);
//     }
