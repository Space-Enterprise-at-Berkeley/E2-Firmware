
#include <Arduino.h>

#include <EspComms.h>
#include <Common.h>

char rs485Buffer[sizeof(Comms::Packet)];
int cnt = 0;
int indicatorDuty = 500;
int indicatorPeriod = 1000;
int indicatorLastTime = 0;

unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 5;

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  // Comms::init(); // takes care of Serial.begin()
  Serial.begin(115200);
  Serial.print("HELLO");
  Serial1.begin(115200);
  Serial1.setPins(17, 18);
  Serial.print("HELLO");

  // while(1) {
  //   // CAP START
  //   unsigned long currentMillis = millis();
  //   if (currentMillis - previousMillis >= interval) {
  //     previousMillis = currentMillis;
  //     Serial.println("HELLOOOO WORLD");
  //   }
  //   int timeNow = currentMillis = millis();
  //   if(timeNow - indicatorLastTime >= indicatorPeriod) {
  //     indicatorLastTime = timeNow;
  //   }
  //   delay(200);
  // }
}

void loop() {
  unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.println("HELLOOOO WORLD");
    }
    int timeNow = currentMillis = millis();
    if(timeNow - indicatorLastTime >= indicatorPeriod) {
      indicatorLastTime = timeNow;
    }
    delay(200);
}
