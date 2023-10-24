#include <Common.h>
#include <TeensyComms.h>

#include <Arduino.h>

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define TEST_SERIAL Serial1 // TODO: verify this
#define E_REG_TEST_TRANSMIT 19
#define E_REG_TEST_RECEIVE 18

char loxBuffer[sizeof(Comms::Packet)];
int loxCnt = 0;

void setup() {
    // hardware setup
    Serial.begin(115200);
    TEST_SERIAL.begin(115200);

    Comms::initComms();

    Serial.println("SETUP");

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    pinMode(E_REG_TEST_TRANSMIT, OUTPUT);
    pinMode(E_REG_TEST_RECEIVE, OUTPUT);

    digitalWriteFast(E_REG_TEST_TRANSMIT, LOW);
    digitalWriteFast(E_REG_TEST_RECEIVE, LOW);
    delay(200);
}

void loop() {
        Serial.println("loop");
        while(TEST_SERIAL.available() && loxCnt < 256) {
            Serial.println("serial available");
            TEST_SERIAL.write(loxBuffer, loxCnt);
            loxBuffer[loxCnt] = TEST_SERIAL.read();
            if(loxBuffer[loxCnt] == '\n') {
                Serial.println("got newline");
            }
            loxCnt++;
        }
        if(loxCnt == 256) {
            Serial.println("RESETTING LOX BUFFER\n");
            loxCnt = 0;
        }
        delay(200);
    }
