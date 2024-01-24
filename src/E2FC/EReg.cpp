#include "EReg.h"

namespace EReg {
    char rs422Buffer[sizeof(Comms::Packet)];
    uint8_t cnt = 0;

    void initEReg() {
        pinMode(HAL::RX1, OUTPUT);
        pinMode(HAL::TX1, OUTPUT);
        digitalWrite(HAL::RX1, LOW);
        digitalWrite(HAL::TX1, HIGH);
    }
    
    uint32_t sampleEReg() {
        cnt = 0;
        while(RS422_SERIAL.available()) {
            rs422Buffer[cnt] = RS422_SERIAL.read();
            if(rs422Buffer[cnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&rs422Buffer;
                if(Comms::verifyPacket(packet)) {
                    DEBUG("Found ereg packet with ID ");
                    DEBUG(packet->id);
                    DEBUG('\n');
                    Comms::emitPacket(packet);
                    Comms::emitPacket(packet, &RADIO_SERIAL, "\r\n\n", 3);
                    break;
                }
            }
            cnt++;
        }
        return 40 * 1000;
    }

};

// #define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

// #define TEST_SERIAL Serial3 // TODO: verify this
// #define RECEIVE_PIN 18
// #define TRANSMIT_PIN 19

// char buffer[sizeof(Comms::Packet)];
// char buffer2[sizeof(Comms::Packet)];
// int cnt = 0;

// void setup() {
//     // hardware setup
//     // Serial.begin(115200);
//     // TEST_SERIAL.begin(115200);

//     // Comms::initComms();
//     pinMode(HAL::RX1, OUTPUT);
//     pinMode(HAL::TX1, OUTPUT);

//     // digitalWriteFast(HAL::RX1, LOW);
//     // digitalWriteFast(HAL::TX1, HIGH);

//     // for (int i = 0; i < sizeof(Comms::Packet); i++) {
//     //     buffer2[i] = 84;
//     // }
//     // TRANSMITS T
// }

// void loop() {
//         while(RS422_SERIAL.available() && cnt < 256) {
//             Serial.println("AVAILABLEEEE - teensy side");
//             buffer[cnt] = TEST_SERIAL.read();
//             Serial.print("printing from ereg --> teensy?: ");
//             Serial.println(buffer[cnt]);
//             TEST_SERIAL.write(buffer2, cnt);
//             // if(cnt == 0 && ((uint8_t)buffer[0] != 23)) {
//             //     Serial.print((uint8_t)buffer[0]);
//             //     Serial.print(" ");
//             //     continue;
//             // }
//             // if(buffer[cnt] == '\n') {
//             //     Serial.println("got newline");
//             //     Comms::Packet *packet = (Comms::Packet *)&buffer;
//             //     if(Comms::verifyPacket(packet)) {
//             //         cnt = 0;
//             //         Serial.print("EREG: ");
//             //         Serial.print(packet->id);
//             //         Serial.print(" : ");
//             //         Serial.print(Comms::packetGetFloat(packet, 0));
//             //         Serial.print("\n");
//             //         Comms::emitPacket(packet);
//             //         break;
//             //     }
//             // }
//             cnt++;
//         }
//         if(cnt == 256) {
//             Serial.println("RESETTING BUFFER\n");
//             cnt = 0;
//         }
//         delay(200);
//     }