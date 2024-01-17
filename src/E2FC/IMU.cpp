

#include "IMU.h"

float acceleration = 0;

float accel_x = 0;
float accel_z = 0;

float accel_x_2 = 0;
float accel_y_2 = 0;
float accel_z_2 = 0;

Adafruit_LSM6DSO32 dso32;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

SparkFun_KX134 kx134;

namespace FlightStatus 
{ 
  float get_acceleration_x() {
    float output;
    dso32.getEvent(&accel, &gyro, &temp);
    output = accel.acceleration.x;
    return output;
  }

  float get_acceleration_y() {
    float output;
    dso32.getEvent(&accel, &gyro, &temp);
    output = accel.acceleration.y;
    return output;
  }

  float get_acceleration_z() {
    float output;
    dso32.getEvent(&accel, &gyro, &temp);
    output = accel.acceleration.z;
    return output;
  }

  float get_gyro_x() {
    float output;
    dso32.getEvent(&accel, &gyro, &temp);
    output = gyro.gyro.x;
    return output;
  }

  float get_gyro_y() {
    float output;
    dso32.getEvent(&accel, &gyro, &temp);
    output = gyro.gyro.y;
    return output;
  }

  float get_gyro_z() {
    float output;
    dso32.getEvent(&accel, &gyro, &temp);
    output = gyro.gyro.z;
    return output;
  }

  void update_accel_2() { 
    if (kx134.dataReady())
    {
      kx134.getAccelData(&kxData);
    }
    accel_x_2 = kxData.xData;
    accel_y_2 = kxData.yData;
    accel_z_2 = kxData.zData;
  }
  
  unsigned long lastExecutedMillis = 0; // create a variable to save the last executed time

  uint32_t updateRate = 100 * 1000;

  void init(){

    if (!dso32.begin_I2C()) {
      while (1) {
        delay(10);
      }
    }

    dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_16_G);
    dso32.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS );
    dso32.setAccelDataRate(LSM6DS_RATE_3_33K_HZ);
    dso32.setGyroDataRate(LSM6DS_RATE_3_33K_HZ);

    if (!kx134.begin()) {
      Serial.println("Could not communicate with the the KX13X. Freezing.");
      while (1);
    }

    // if (kx134.softwareReset())
    //   Serial.println("Reset.");

    // Give some time for the accelerometer to reset.
    // It needs two, but give it five for good measure.
    delay(2);
    kx134.enableAccel(false);
    kx134.setRange(SFE_KX134_RANGE64G);         // 16g for the KX134
    kx134.enableDataEngine(); // Enables the bit that indicates data is ready.
    // kx134.setOutputDataRate(); // Default is 50Hz
    kx134.enableAccel();

  
  }

  // Packet definitions
  Comms::Packet imuPacket = {.id = 3};

  uint32_t updateFlight() { 
    acceleration = get_acceleration_y();
    update_accel_2();


    accel_x = get_acceleration_x();
    accel_z = get_acceleration_z();

    float gyro_x = get_gyro_x();
    float gyro_y = get_gyro_y();
    float gyro_z = get_gyro_z();

    acceleration = acceleration / 9.81; // convert to g

    if (0.8 < acceleration/9.81 & 1.2 > acceleration/9.81) {
     acceleration = 0;
    }

    if (acceleration < 0) {
     acceleration = 0;
    }
    Serial.print("Acceleration:"); Serial.print(acceleration); Serial.println("  ");

    // for (int i = 0; i < 10; i++) { 
      imuPacket.len = 0;
      Comms::packetAddFloat(&imuPacket, accel_x);
      Comms::packetAddFloat(&imuPacket, acceleration);
      Comms::packetAddFloat(&imuPacket, accel_z);
      Comms::packetAddFloat(&imuPacket, gyro_x);
      Comms::packetAddFloat(&imuPacket, gyro_y);
      Comms::packetAddFloat(&imuPacket, gyro_z);
      Comms::packetAddFloat(&imuPacket, accel_x_2);
      Comms::packetAddFloat(&imuPacket, accel_y_2);
      Comms::packetAddFloat(&imuPacket, accel_z_2);

      // emit the packet
      Comms::emitPacketToGS(&imuPacket);
      Radio::forwardPacket(&imuPacket);
// }
    
    return updateRate;
  }

}