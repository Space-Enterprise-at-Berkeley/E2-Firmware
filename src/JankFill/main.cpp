#include <Arduino.h>
#include <Common.h>
#include <FDC2214.h>
#include <Wire.h>
#include <TeensyComms.h>
#include <CircularBuffer.h>


FDC2214 _capSens;

Comms::Packet capPacket = {.id = 21};


float runningAverage(float total, int numSamples){
  if (numSamples == 0) {
    return 0;
  }
  return total / numSamples;
}

uint32_t print_Cap() {
    float capValue = _capSens.readCapacitance(00);
    float sensor0 = _capSens.readSensor(00);
    float sensor1 = _capSens.readSensor(01);
    float refValue = _capSens.readCapacitance(01);

    Serial.print("Capacitance: ");
    Serial.println(capValue);
    Serial.print("Sensor 0: ");
    Serial.println(sensor0);
    Serial.print("Sensor 1: ");
    Serial.println(sensor1);
    Serial.print("Reference: ");
    Serial.println(refValue);
    Serial.println();
    return 200 * 1000;
}

Comms::Packet hello = {.id = 0, .len = 0};
uint32_t helloPacket() {
    hello.len = 0;
    Comms::packetAddFloat(&hello, 420.69);
    Comms::emitPacketToGS(&hello);
    Serial.println("Packet sent");
    return 100 * 1000;
}

bool on = false;
uint32_t oscillate1819() {
    digitalWrite(18, on);
    digitalWrite(19, !on);
    on = !on;
    return 5000 * 1000;
}

// samhitag3 added variables for maintaining running average
int numSamples = 0;
int oldestSampleIndex = 0;
int const sampleSize = 100;
float samples[sampleSize];
float total = 0;
float baseline = 0;

const uint8_t logSecs = 5;
const long readInterval = 25; //ms
CircularBuffer<float, (logSecs * 1000 / readInterval)> capBuffer;
CircularBuffer<float, (logSecs * 1000 / readInterval)> refBuffer;
CircularBuffer<float, (logSecs * 1000 / readInterval)> correctedBuffer;

uint32_t readCap() {

    // samhitag3 changed readCapacitance input value
    float capValue = _capSens.readCapacitance(00);
    // samhitag3 defined refValue, sensor0, sensor1
    float sensor0 = _capSens.readSensor(00);
    float sensor1 = _capSens.readSensor(01);
    float refValue = _capSens.readCapacitance(01);

    // samhitag3 passed data to running average method
    if (numSamples < sampleSize) {
      if (numSamples == 0) {
        baseline = refValue;
      }
      samples[numSamples] = refValue;
      total += refValue;
      if (numSamples == sampleSize - 1) {
        baseline = total / numSamples;
      }
      numSamples++;
    }

    if(capValue < 0.0) {
      // error reading from sensor
      Serial.println("Error reading from sensor");
    } 
    Serial.print("Cap value: ");
    Serial.println(capValue);

    capBuffer.push(capValue);
    refBuffer.push(refValue);

    float avgCap = 0;
    float avgRef = 0;
    for (int i = 0; i < capBuffer.size(); i++){
      avgCap += capBuffer[i];
      avgRef += refBuffer[i];
    }
    avgCap /= capBuffer.size();
    avgRef /= refBuffer.size();

    //float tempValue = _tempSens.readTemperature();
    float corrected = _capSens.correctedCapacitance(avgRef, baseline);

    correctedBuffer.push(corrected);
    float avgCorrected = 0;
    for (int i = 0; i < correctedBuffer.size(); i++){
      avgCorrected += correctedBuffer[i];
    }
    avgCorrected /= correctedBuffer.size();

    //print stuff
    Serial.print("S0: ");
    Serial.print(sensor0);
    Serial.print("\t");
    Serial.print("S1: ");
    Serial.print(sensor1);
    Serial.print("\t");
    Serial.print("Cap: ");
    Serial.print(capValue);
    Serial.print("\t");
    Serial.print("Ref: ");
    Serial.println(refValue);
    Serial.print("Avg cap: ");
    Serial.print(avgCap);
    Serial.print("\t");
    Serial.print("Avg ref: ");
    Serial.print(avgRef);
    Serial.print("\t");
    Serial.print("Corrected: ");
    Serial.print(corrected);
    Serial.print("\t");
    Serial.print("Avg corrected: ");
    Serial.println(avgCorrected);

    capPacket.len = 0;
    
    Comms::packetAddFloat(&capPacket, corrected);
    Comms::packetAddFloat(&capPacket, avgCap);
    Comms::packetAddFloat(&capPacket, refValue);
    Comms::emitPacketToGS(&capPacket);

    return readInterval*1000;
}


Task taskTable[] = {
    //{print_Cap, 0, true},
    //{helloPacket, 0, true}
    //{oscillate1819, 0, true}
    {readCap, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
    Serial.begin(115200);
    Comms::initComms();


    Wire.begin();
    _capSens = FDC2214();
    _capSens.begin(0x2A, &Wire);

    /* pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    digitalWrite(18, LOW);
    digitalWrite(19, LOW); */
 
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
    //Comms::processWaitingPackets();
  }
}

void loop() {} // unused
