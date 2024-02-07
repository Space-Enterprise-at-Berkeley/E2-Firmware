/*
#include <MAX22201.h>

MAX22201 m;

void setup() {
  m.init(47, 8);
  Serial.begin(921600);
}

void loop() {
  Serial.println("going");
  delay(500);
  m.backwards();
  delay(500);
  m.stop();
}
*/
/*
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

void setup() {
  Serial.begin(921600);
  while (!Serial);
  Serial.println("Adafruit BMP388 / BMP390 test");

// initialize I2C
    Wire.begin(0, 1); 
    Wire.setClock(100000);

  if (!bmp.begin_I2C(0x77)) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop() {
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bmp.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
  delay(2000);
}
*/

// DSO32 Works

/*
#include <Wire.h>
#include <Adafruit_LSM6DSO32.h>
Adafruit_LSM6DSO32 dso32;

void setup() {
    Serial.begin(921600);
    Wire.begin(1, 2);
    //Wire.setClock(100000);
    // FlightSensors
        if (!dso32.begin_I2C(0x6B)) {
            while (1) {
            Serial.println("sad");
            }
        }


        dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_16_G);
        dso32.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS );
        dso32.setAccelDataRate(LSM6DS_RATE_3_33K_HZ);
        dso32.setGyroDataRate(LSM6DS_RATE_3_33K_HZ);
}

float accel, gyro, temp;

void loop() {
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    dso32.getEvent(&accel, &gyro, &temp);

    Serial.print("\t\tTemperature ");
    Serial.print(temp.temperature);
    Serial.println(" deg C");

    Serial.print("\t\tAccel X: ");
    Serial.print(accel.acceleration.x);
    Serial.print(" \tY: ");
    Serial.print(accel.acceleration.y);
    Serial.print(" \tZ: ");
    Serial.print(accel.acceleration.z);
    Serial.println(" m/s^2 ");

    Serial.print("\t\tGyro X: ");
    Serial.print(gyro.gyro.x);
    Serial.print(" \tY: ");
    Serial.print(gyro.gyro.y);
    Serial.print(" \tZ: ");
    Serial.print(gyro.gyro.z);
    Serial.println(" radians/s ");
    Serial.println();

    delay(100);
}
*/

//MS5607 Works

/*
#include <Wire.h>
#include "MS5607.h"

MS5607 P_Sens = MS5607(0x76);
void setup() {
    Serial.begin(921600);
    Wire.begin(1, 2);
    if(!P_Sens.begin()){
        Serial.println("Error in Communicating with sensor, check your connections!");
    }
    else{
    Serial.println("MS5607 initialization successful!");
  }
}

float P_val,T_val,H_val;
void loop(void){
  if(P_Sens.readDigitalValue()){
    T_val = P_Sens.getTemperature();
    P_val = P_Sens.getPressure();
    H_val = P_Sens.getAltitude();
  }else{
    Serial.println("Error in reading digital value in sensor!");
  }

  Serial.print("Temperature :  ");
  Serial.print(T_val);
  Serial.println(" C");
  Serial.print("Pressure    :  ");
  Serial.print(P_val);
  Serial.println(" mBar");
  Serial.print("Altitude    :  ");
  Serial.print(H_val);
  Serial.println(" meter");

  delay(1000);
}
*/

// BMP390 Works

/*
#include <Wire.h>
#include "Adafruit_BMP3XX.h"


#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

void setup() {
    Serial.begin(921600);
    Wire.begin(1, 2);
    if (!bmp.begin_I2C(0x77)) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop() {
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bmp.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
  delay(2000);
}
*/

/*
#include <Wire.h>
#include "Adafruit_BMP3XX.h"

#include "SparkFun_KX13X.h"
SparkFun_KX132 kxAccel;
outputData myData; // Struct for the accelerometer's data

void setup() {
    Serial.begin(921600);
    Wire.begin(1, 2);
    if (!kxAccel.begin(&Wire, 0x1F))
    {
    Serial.println("Could not communicate with the the KX13X. Freezing.");
    while (1)
    ;
    }

    Serial.println("Ready.");

    if (kxAccel.softwareReset())
    Serial.println("Reset.");

    // Give some time for the accelerometer to reset.
    // It needs two, but give it five for good measure.
    delay(5);

    // Many settings for KX13X can only be
    // applied when the accelerometer is powered down.
    // However there are many that can be changed "on-the-fly"
    // check datasheet for more info, or the comments in the
    // "...regs.h" file which specify which can be changed when.
    kxAccel.enableAccel(false);

    kxAccel.setRange(SFE_KX132_RANGE16G); // 16g Range
    // kxAccel.setRange(SFE_KX134_RANGE16G);         // 16g for the KX134

    kxAccel.enableDataEngine(); // Enables the bit that indicates data is ready.
    // kxAccel.setOutputDataRate(); // Default is 50Hz
    kxAccel.enableAccel();
}

void loop()
{
  // Check if data is ready.
  if (kxAccel.dataReady())
  {
    kxAccel.getAccelData(&myData);
    Serial.print("X: ");
    Serial.print(myData.xData, 4);
    Serial.print(" Y: ");
    Serial.print(myData.yData, 4);
    Serial.print(" Z: ");
    Serial.print(myData.zData, 4);
    Serial.println();
  }
  delay(20); // Delay should be 1/ODR (Output Data Rate), default is 1/50ODR
}

*/

#include <Common.h>
#include <EspComms.h>
#include <Wire.h>
#include <Arduino.h>

#include "WiFiCommsLite.h"
#include "Ducers.h"
#include "FlightSensors.h"
#include "Actuators.h"
#include "ChannelMonitor.h"
#include "Automation.h"
#include "ReadPower.h"
#include "BlackBox.h"
#include "Radio.h"
#include "EReg.h"


uint32_t print_task() { 
  Ducers::print_ptSample();
  return 1000 * 1000;
}

Task taskTable[] = {
  // Ducers
  {Ducers::task_ptSample, 0, true},
  {print_task, 0, true},
  {FlightSensors::task_barometers, 0, true},
  {FlightSensors::task_accels, 0, true},
  {ChannelMonitor::task_readChannels, 0, true},
  {AC::task_actuationDaemon, 0, true},
  {AC::task_actuatorStates, 0, true},
  {Automation::task_sendAutoventConfig, 0, true},
  {Power::task_readSendPower, 0, true},
  {WiFiComms::task_WiFiDaemon, 0, true}
  //automation config
  //launch daemon?
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here

  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);

  Power::init();
  Comms::init(); // takes care of Serial.begin()
  EREG_Comms::init();
  WiFiComms::init();
  initWire();
  Ducers::init();
  AC::init();
  FlightSensors::init();
  ChannelMonitor::init(7, 6, 5, 3, 4);
  Automation::init();
  BlackBox::init();
  Radio::initRadio();

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
    EREG_Comms::processAvailableData();
    WiFiComms::processWaitingPackets();
    Radio::processRadio();
  }
}

void loop() {} // unused