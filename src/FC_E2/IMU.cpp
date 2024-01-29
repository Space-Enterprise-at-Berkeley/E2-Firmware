#include "IMU.h"
#include <Wire.h>
#include "Adafruit_BMP3XX.h"
#include "MS5607.h"
#include <Adafruit_LSM6DSO32.h>
#include "SparkFun_KX13X.h"


#define SEALEVELPRESSURE_HPA (1013.25)
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

namespace IMU {

    Adafruit_BMP3XX bmp;
    MS5607 fancyBarometer = MS5607(0x76);
    Comms::Packet baroPacket = {.id = 3};

    Adafruit_LSM6DSO32 lowg_accel;
    SparkFun_KX132 highg_accel;

    Comms::Packet accelPacket = {.id = 2};

    // assumes initWire has been called
    void init() {

        // BMP 390 SETUP
        if (!bmp.begin_I2C(0x77)) {   // hardware I2C mode, can pass in address & alt Wire
            Serial.println("Error communicating with BMP390, check your connections!");
            while (1);
        }
        // Set up oversampling and filter initialization
        bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
        bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
        bmp.setOutputDataRate(BMP3_ODR_50_HZ);

        // MS5607 SETUP
        if(!fancyBarometer.begin()){
            Serial.println("Error communicating with MS5607, check your connections!");
        }

        // LSM6DS032 SETUP
        if (!lowg_accel.begin_I2C(0x6B)) {
            Serial.println("Error communicating with LSM6DS032, check your connections!");
        }
        // LOW G Accelerometer settings : 16G
        lowg_accel.setAccelRange(LSM6DSO32_ACCEL_RANGE_16_G);
        lowg_accel.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS );
        lowg_accel.setAccelDataRate(LSM6DS_RATE_3_33K_HZ);
        lowg_accel.setGyroDataRate(LSM6DS_RATE_3_33K_HZ);

        //KX13X SETUP
        if (!highg_accel.begin(0x1F))
        {
            Serial.println("Error communicating with KX13X, check your connections!");
        }

        highg_accel.softwareReset();
        delay(5);

        // must disable accel to set settings
        highg_accel.enableAccel(false);
        highg_accel.setRange(SFE_KX134_RANGE64G); // 64g Range
        highg_accel.enableDataEngine(); // Enables the bit that indicates data is ready.
        highg_accel.enableAccel();
    }


    uint32_t task_barometers() {
        // BAROMETER PACKET
        baroPacket.len = 0;

        // BMP 390
        bmp.performReading();
        Comms::packetAddFloat(&baroPacket, bmp.readAltitude(SEALEVELPRESSURE_HPA));
        Comms::packetAddFloat(&baroPacket, bmp.temperature);
        Comms::packetAddFloat(&baroPacket, bmp.pressure / 100.0);

        float MS5607_temp = 0;
        float MS5607_pressure = 0;
        float MS5607_altitude = 0;

        // MS5607
        if(fancyBarometer.readDigitalValue()){
            MS5607_temp = fancyBarometer.getTemperature();
            MS5607_pressure = fancyBarometer.getPressure();
            MS5607_altitude = fancyBarometer.getAltitude();
        }
        else{
            Serial.println("MS5607 Error");
        }

        Comms::packetAddFloat(&baroPacket, MS5607_altitude);
        Comms::packetAddFloat(&baroPacket, MS5607_temp);
        Comms::packetAddFloat(&baroPacket, MS5607_pressure);

        Comms::emitPacketToGS(&baroPacket);
        return 50 * 1000;
    }

    uint32_t task_accels() {
        // ACCELEROMETER PACKET
        accelPacket.len = 0;

        // LSM6DS032 (16g, lowg)
        sensors_event_t accel;
        sensors_event_t gyro;
        sensors_event_t temp;
        lowg_accel.getEvent(&accel, &gyro, &temp);

        Comms::packetAddFloat(&accelPacket, accel.acceleration.x);
        Comms::packetAddFloat(&accelPacket, accel.acceleration.y);
        Comms::packetAddFloat(&accelPacket, accel.acceleration.z);
        Comms::packetAddFloat(&accelPacket, gyro.gyro.x);
        Comms::packetAddFloat(&accelPacket, gyro.gyro.y);
        Comms::packetAddFloat(&accelPacket, gyro.gyro.z);

        // KX134
        outputData KX134_data; // Struct for the accelerometer's data
        if (highg_accel.dataReady())
        {
            highg_accel.getAccelData(&KX134_data);
        }

        Comms::packetAddFloat(&accelPacket, KX134_data.xData);
        Comms::packetAddFloat(&accelPacket, KX134_data.yData);
        Comms::packetAddFloat(&accelPacket, KX134_data.zData);

        Comms::emitPacketToGS(&accelPacket);
        return 50 * 1000;
    }


}