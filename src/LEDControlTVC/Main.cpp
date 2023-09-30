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

uint16_t i = 0;

uint32_t LEDTest(){
  
  leds[i] = CRGB(0,255,0);
  i = (i + 1) % NUM_LEDS; 
  FastLED.show(); 
  Serial.println(i);
  return 50 * 1000;
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

Task taskTable[] = {
  {rainbowMode, 0, true},
  {tvcMode, 0, false},
  {LEDTest, 0, false},
  {helloWorld, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void changeMode(Comms::Packet pckt, uint8_t ip){
  uint8_t mode = Comms::packetGetUint8(&pckt, 0);
  for (uint32_t i = 0; i < TASK_COUNT; i++){
    taskTable[i].enabled = false;
  }
  taskTable[mode].enabled = true;
}

void updateTVCval(Comms::Packet pckt, uint8_t ip){
  uint32_t a = Comms::packetGetFloat(&pckt, 0);
  tvcX = *(int*) &a;
  a = Comms::packetGetFloat(&pckt, 4);
  tvcY = *(int*) &a;
  Serial.printf("got updatetvcval x: %d y: %d\n", tvcX, tvcY);
}

void setup() {
  // setup stuff here
  Serial.begin(115200);
  Comms::init(); // takes care of Serial.begin()
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  pinMode(DATA_PIN, OUTPUT);

  Comms::registerCallback(100, changeMode);
  Comms::registerCallback(42, updateTVCval);

  // while(1){
  //   digitalWrite(DATA_PIN, HIGH);
  //   delay(1000);
  //   digitalWrite(DATA_PIN, LOW);
  //   delay(1000);
  // }
  
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
        taskTable[i].nexttime = ticks + taskTable[i].taskCall();
        }
      }
    }
    Comms::processWaitingPackets();
  }
}

void loop() {} // unused





// #define LOX_BOT 112
// #define LOX_TOP 137
// #define FUEL_BOT 107
// #define FUEL_TOP 157

// #define NUM_LEDS_TOTAL 144
// #define DATA_PIN 15
// #define NUM_LEDS_LOX
// #define NUM_LEDS_FUEL

// #define TANK_OFFSET 
    //distance to bottom of fuel = offset + lox length

// CRGB lox_color(0, 50, 98);
// CRGB fuel_color(100, 60, 0);

// CRGB loxLed[NUM_LEDS];
// CRGB fuelLed[NUM_LEDS];
// CRGB leds2[NUM_LEDS2];

// void loxLedPacketHandler(Comms::Packet tmp, uint8_t ip);
// void fuelLedPacketHandler(Comms::Packet tmp, uint8_t ip);

// void setup()
// {
//   Serial.begin(115200);
//   FastLED.addLeds<NEOPIXEL, LOX_DATA_PIN>(loxLed, NUM_LEDS);
//   FastLED.addLeds<NEOPIXEL, FUEL_DATA_PIN>(fuelLed, NUM_LEDS);
//   FastLED.addLeds<NEOPIXEL, DATA_PIN2>(leds2, NUM_LEDS2);

//   //Comms::initComms();
//   Comms::registerCallback(21, loxLedPacketHandler);
//   Comms::registerCallback(22, fuelLedPacketHandler);
// }

// unsigned long previousMillis = 0;
// const long interval = 10;

// bool loxCapConnected = false;
// bool fuelCapConnected = false;
// int startHue = 0;

// void loop()
// {
//   //ArduinoOTA.handle();
//   unsigned long currentMillis = millis();
//   if (currentMillis - previousMillis >= interval)
//   {
//     Comms::processWaitingPackets();
//     startHue += 1;
//     startHue = startHue % 256;
//     if (!loxCapConnected)
//     {
//       fill_rainbow(loxLed, NUM_LEDS, (startHue*-1)%256, 1);
//     }
//     if (!fuelCapConnected)
//     {
//       fill_rainbow(fuelLed, NUM_LEDS, startHue, 1);
//     }
// #ifdef LOX
//     fill_rainbow(leds2, NUM_LEDS2, startHue, 255 / NUM_LEDS);
// #endif
//     FastLED.show();
//   }
// }

// void loxLedPacketHandler(Comms::Packet tmp)
// {
//   loxCapConnected = true;
//   float capValue = Comms::packetGetFloat(&tmp, 0);

//   float scaledLED = (capValue - LOX_BOT) / (LOX_TOP - LOX_BOT) * NUM_LEDS;
//   if (scaledLED < 0)
//   {
//     for (int i = 0; i < NUM_LEDS; i++)
//     {
//       loxLed[i] = CRGB(0, 0, 0);
//     }
//   }
//   else if (scaledLED > NUM_LEDS)
//   {
//     for (int i = 0; i < NUM_LEDS; i++)
//     {
//       loxLed[i] = lox_color;
//     }
//   }
//   else
//   {
//     int full_leds = floor(scaledLED);
//     for (int i = 0; i < full_leds; i++)
//     {
//       loxLed[i] = lox_color;
//     }
//     for (int i = full_leds; i < NUM_LEDS; i++)
//     {
//       loxLed[i] = CRGB(0, 0, 0);
//     }
//     float overflow = scaledLED - full_leds;
//     loxLed[full_leds] = CRGB(scale8(lox_color.r, overflow * 256), scale8(lox_color.g, overflow * 256), scale8(lox_color.b, overflow * 256));
//   }
//   blur1d(loxLed, NUM_LEDS, 172);
//   blur1d(loxLed, NUM_LEDS, 172);
//   blur1d(loxLed, NUM_LEDS, 172);
//   blur1d(loxLed, NUM_LEDS, 172);
//   FastLED.show();
// }

// void fuelLedPacketHandler(Comms::Packet tmp)
// {
//   fuelCapConnected = true;
//   float capValue = Comms::packetGetFloat(&tmp, 0);

//   float scaledLED = (capValue - FUEL_BOT) / (FUEL_TOP - FUEL_BOT) * NUM_LEDS;
//   if (scaledLED < 0)
//   {
//     for (int i = 0; i < NUM_LEDS; i++)
//     {
//       fuelLed[i] = CRGB(0, 0, 0);
//     }
//   }
//   else if (scaledLED > NUM_LEDS)
//   {
//     for (int i = NUM_LEDS - 1; i >= 0; i--)
//     {
//       fuelLed[i] = fuel_color;
//     }
//   }
//   else
//   {
//     int full_leds = floor(scaledLED);
//     for (int i = NUM_LEDS - 1; i > NUM_LEDS - 1 - full_leds; i--)
//     {
//       fuelLed[i] = fuel_color;
//     }
//     for (int i = NUM_LEDS - 1 - full_leds; i >= 0; i--)
//     {
//       fuelLed[i] = CRGB(0, 0, 0);
//     }
//     float overflow = scaledLED - full_leds;
//     fuelLed[NUM_LEDS - 1 - full_leds] = CRGB(scale8(fuel_color.r, overflow * 256), scale8(fuel_color.g, overflow * 256), scale8(fuel_color.b, overflow * 256));
//   }
//   blur1d(fuelLed, NUM_LEDS, 172);
//   blur1d(fuelLed, NUM_LEDS, 172);
//   blur1d(fuelLed, NUM_LEDS, 172);
//   blur1d(fuelLed, NUM_LEDS, 172);
//   FastLED.show();
// }