// //have correct pin values, top/bottom values





#include <Arduino.h>
#include <EspComms.h>
#include <Common.h>
#include "FastLED.h"
#include <Arduino.h>
#include <math.h>

const uint16_t NUM_LEDS = 500;
const uint8_t DATA_PIN = 15;

CRGB leds[NUM_LEDS];


uint32_t helloWorld(){
  Comms::Packet hello = {.id = 3};
  Comms::emitPacketToGS(&hello);
  return 100 * 1000;
}

uint16_t i = 70;

uint32_t LEDTest(){
  
  leds[i] = CRGB(0,255,0);
  i = (i + 1) % NUM_LEDS; 
  FastLED.show(); 
  Serial.println(i);
  return 200 * 1000;
}

uint8_t r = 0;
uint32_t rainbowMode(){
  //Serial.println("Rainbow cycle");
  for (uint16_t i = 0; i < NUM_LEDS; i++){
    leds[i] = CHSV(r+i, 255, 255);
  }
  r++;
  FastLED.show();
  return 40 * 1000;
}

int tvcX = 0;
int tvcY = 0;
uint16_t circleLEDcount = 50;
uint16_t circleStartLED = 5;
uint32_t tvcMode() {
  float radius = min(sqrt(tvcX*tvcX + tvcY*tvcY) / 495, 1.0);
  float angle = atan2(tvcY,tvcX);

  if (radius < 0.05) {
    for (uint16_t i = circleStartLED; i < circleStartLED+circleLEDcount; i++){
      leds[i] = CHSV(0, 255, 255);
    }
  } else {
    int ledPos = (int) (angle/(2*3.141) * circleLEDcount) + circleStartLED;
    int ledRange = (int)(radius/2 * circleLEDcount);
    for (uint16_t i = 0; i < ledRange; i++) {
      leds[ledPos + i] = CHSV(0, 255,  255 - (255*i)/ledRange);
      leds[ledPos - i] = CHSV(0, 255,  255 - (255*i)/ledRange);
    }
  }
  FastLED.show();
  return 40*1000;

}
uint32_t idleMode();
Task taskTable[] = {
  // {rainbowMode, 0, false},
  // {tvcMode, 0, false},
  // {LEDTest, 0, false},
  {idleMode, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))


// 54
// 109
// 160
// 206
// 247
// 284
int16_t startIndices[8] = {-5, 54, 109, 159, 205, 246, 282, 318};
void setAngleHue(float angle) {

  while (angle > (2 * 3.14)) {
    angle -= (2 * 3.14);
  }
  while (angle < 0) {
    angle += (2 * 3.14);
  }

  for (int i = 0; i < 7; i++) {
    int startInd = startIndices[i];
    int stopInd = startIndices[i+1];
    int numLEDs = stopInd - startInd;

    for (int j = startInd; j < stopInd; j++) {

      uint8_t startHue = (angle / (2 * 3.14)) * 255; 
      if (j >= 0) {
        uint8_t h = (uint8_t) ((255.0 / ((float) numLEDs)) * (j - startInd));
        h += startHue;
        uint8_t realColor = (uint8_t) 88.0 * (((float)h) / 255.0);
        if (realColor > 44) {
          realColor = 88 - realColor;
        }
        uint8_t r = (uint8_t) ((float)realColor) * (255.0 / 44.0);
        uint8_t b = (uint8_t) (44.0 - (float)realColor) * (255.0 / 44.0) * 0.5;

        // Serial.printf("h: %d, j: %d\n", realColor, j);
        leds[j] = CRGB(r, 0, b);

      }
    }

  }
  FastLED.show();
}

// void changeMode(Comms::Packet pckt, uint8_t ip){
//   uint8_t mode = Comms::packetGetUint8(&pckt, 0);
//   for (uint32_t i = 0; i < TASK_COUNT; i++){
//     taskTable[i].enabled = false;
//   }
//   taskTable[mode].enabled = true;
// }

void updateTVCval(Comms::Packet pckt, uint8_t ip){
  // taskTable[0].enabled = false;
  // taskTable[1].enabled = true;
  uint32_t a = Comms::packetGetUint32(&pckt, 0);
  tvcX = *(int*) &a;
  a = Comms::packetGetUint32(&pckt, 4);
  tvcY = *(int*) &a;
  Serial.printf("x: %d, y: %d\n", tvcX, tvcY);
  float radius = min(sqrt(tvcX*tvcX + tvcY*tvcY) / 495, 1.0);
  float angle = atan2(tvcY,tvcX);
  if (radius > 10) {
    taskTable[0].enabled = false;
    setAngleHue(angle);
  } else {
    taskTable[0].enabled = true;
  }
  Serial.printf("got updatetvcval angle: %f\n", angle);

}

uint8_t idlePtr = 0;
uint32_t idleMode() {
  for (int i = 0; i < 7; i++) {
    int startInd = startIndices[i];
    int stopInd = startIndices[i+1];
    int numLEDs = stopInd - startInd;

    for (int j = startInd; j < stopInd; j++) {

      uint8_t startHue = idlePtr + i*(255 / (7 * 2));
      if (j >= 0) {
        uint8_t realColor = (uint8_t) 88.0 * (((float)startHue) / 255.0);
        if (realColor > 44) {
          realColor = 88 - realColor;
        }
        uint8_t r = (uint8_t) ((float)realColor) * (255.0 / 44.0);
        uint8_t b = (uint8_t) (44.0 - (float)realColor) * (255.0 / 44.0) * 0.5;
        // Serial.printf("h: %d, j: %d\n", realColor, j);
        float goldR = (220.0 / 255.0); float goldG = 215.0/255.0;
        leds[j] = CRGB((uint8_t) ((float)r * 1), (uint8_t) (goldG * (float)r * 0), (uint8_t) ((float)b * 1));
      }
    }
  }
  idlePtr += 10;
  idlePtr %= 255;
  FastLED.show();
  return 80 * 1000;
}

void setup() {
  // setup stuff here
  Serial.begin(115200);
  Comms::init(); // takes care of Serial.begin()
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  pinMode(DATA_PIN, OUTPUT);
  Serial.printf("Hi!!\n");

  // Comms::registerCallback(100, changeMode);
  Comms::registerCallback(42, updateTVCval);

  // float angle = 0;
  // while (1) {
  //   setAngleHue(angle);
  //   angle += 1;
  //   delay(1000);
  // }
  // while (1);

  while(1) {
    // main loop here to avoid arduino overhead
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      uint32_t ticks = micros(); // current time in microseconds
      if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
        uint32_t nextTime = taskTable[i].taskCall();
        if (nextTime == 0){
          taskTable[i].enabled = false;
        }
        else {
          taskTable[i].nexttime = ticks + nextTime;
        }
      }
    }
    Comms::processWaitingPackets();
  }
}

void loop() {} // unused
