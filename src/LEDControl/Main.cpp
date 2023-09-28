// //have correct pin values, top/bottom values

#include <Arduino.h>
#include <EspComms.h>
#include <Common.h>
#include "FastLED.h"
#include <Arduino.h>

const uint16_t NUM_LEDS = 500;
const uint8_t DATA_PIN = 15;

const uint8_t TANK_LENGTH = 58;
const uint8_t TANK_SEPERATION = 15;
const uint8_t TANK_END = TANK_LENGTH + TANK_SEPERATION + TANK_LENGTH;

CRGB lox_color(0, 50, 98);
CRGB fuel_color(100, 60, 0);


float lox_capval;
float fuel_capval;
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

float lox_lowerLimit = 150;
float lox_upperLimit = 200;
float fuel_lowerLimit = 150;
float fuel_upperLimit = 200;
bool rIncreasing = true;
uint32_t fillMode(){
  for (uint16_t i = 0; i < TANK_LENGTH; i++){
    if (i < TANK_LENGTH * (lox_capval - lox_lowerLimit) / (lox_upperLimit - lox_lowerLimit)){
      //hover around shades of blue
      //leds[i] = CHSV(180 + r*100/(float)255, 255, 255);
      leds[i] = lox_color;
    }
    else {
      //leds[i] = CRGB(255,0,0);
      leds[i] = CRGB(0,0,0);
    }
  }
  for (uint16_t i = TANK_LENGTH + TANK_SEPERATION; i < TANK_END; i++){
    if (i < TANK_LENGTH * (fuel_capval - fuel_lowerLimit) / (fuel_upperLimit - fuel_lowerLimit) + TANK_LENGTH + TANK_SEPERATION){
      //hover around shades of green
      //leds[i] = CHSV(60 + r*100/(float)255, 255, 255);
      leds[i] = fuel_color;
      
    }
    else {
      //leds[i] = CRGB(255,0,0);
      leds[i] = CRGB(0,0,0);
      
    }
  }
  if (rIncreasing){
    r++;
    if (r == 255){
      rIncreasing = false;
    }
  }
  else {
    r--;
    if (r == 0){
      rIncreasing = true;
    }
  }
  FastLED.show();
  return 40 * 1000;
}

uint8_t lox_bottomLED = 0;
uint8_t lox_topLED = 55;
uint8_t fuel_bottomLED = 75;
uint8_t fuel_topLED = 130;
uint8_t t = 0;
uint8_t s = 0;
uint8_t base_brightness = 150;
uint8_t brightness;
uint32_t combinedMode(){
  //Serial.println("Combined cycle");
  for (uint16_t i = 0; i < NUM_LEDS; i++){
    if ( t - i < 11 && t - i > -11){
      brightness = 255 - (t-i)*(t-i), base_brightness;
    } else {
      brightness = base_brightness;
    }


    if (i >= lox_bottomLED && i <= lox_topLED){
      if ((lox_capval-lox_lowerLimit)/(float)(lox_upperLimit-lox_lowerLimit) > (i-lox_bottomLED)/(float)(lox_topLED - lox_bottomLED)){
        //leds[i] = lox_color;
        leds[i] = CHSV(209, 255, brightness);
      } else {
        leds[i] = CHSV(0, 0, brightness);
      }
    }
    else if (i >= fuel_bottomLED && i <= fuel_topLED){
      if ((fuel_capval - fuel_lowerLimit)/(float)(fuel_upperLimit-fuel_lowerLimit) > (i-fuel_bottomLED)/(float)(fuel_topLED - fuel_bottomLED)){
        //leds[i] = fuel_color;
        leds[i] = CHSV(36, 255, brightness);
      } else {
        leds[i] = CHSV(0, 0, brightness);
      }
    }
    else{
      leds[i] = CHSV(s+i, 255, brightness);
    }
  }
  t++;
  if (t == NUM_LEDS + 80){
    t = 0;
  }
  if (t%10== 0){
    s++;
  }
  FastLED.show();
  return 1 * 1000;
}

void updateLoxCapVal(Comms::Packet pckt, uint8_t ip){
   
     lox_capval = Comms::packetGetFloat(&pckt, 0);
     Serial.println(lox_capval);
   }

  void updateFuelCapVal(Comms::Packet pckt, uint8_t ip){
  
    fuel_capval = Comms::packetGetFloat(&pckt, 0);
    Serial.println(fuel_capval);
  }


// void updateCapVal(Comms::Packet pckt, uint8_t ip){
//   if (ip == 72){
//     lox_capval = Comms::packetGetFloat(&pckt, 0);
//     Serial.println(lox_capval);
//   }
//   else if (ip == 42){
//     fuel_capval = Comms::packetGetFloat(&pckt, 0);
//     Serial.println(fuel_capval);
//   }
// }

Task taskTable[] = {
  {rainbowMode, 0, false},
  {fillMode, 0, false},
  {combinedMode, 0, true},
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

void changeCapBounds(Comms::Packet pckt, uint8_t ip){
   lox_lowerLimit = Comms::packetGetFloat(&pckt, 0);
 lox_upperLimit = Comms::packetGetFloat(&pckt, 4);
 fuel_lowerLimit = Comms::packetGetFloat(&pckt, 8);
 fuel_upperLimit = Comms::packetGetFloat(&pckt, 12);
 Serial.println("Set bounds for lox to " + String(lox_lowerLimit) + " and " + String(lox_upperLimit));
 Serial.println("Set bounds for fuel to " + String(fuel_lowerLimit) + " and " + String(fuel_upperLimit));

}

void ledStartup(){
  for (int i = 0; i < NUM_LEDS; i+=5){
    for (int j = 0; j < 5; j++){
      leds[i+j] = CRGB(255,0,0);
    }
    FastLED.show();
    delay(500);
  }
}

void setup() {
  // setup stuff here
  Serial.begin(115200);
  Comms::init(); // takes care of Serial.begin()
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  pinMode(DATA_PIN, OUTPUT);

  Comms::registerCallback(21, updateLoxCapVal);
  Comms::registerCallback(22, updateFuelCapVal);
  Comms::registerCallback(100, changeMode);
  Comms::registerCallback(101, changeCapBounds);

  ledStartup();

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