#include "IMU.h"


namespace IMU {

    
    
    void initIMU() { 
        if (!imu.begin_I2C()) {
            while (1) {
            delay(10);
            }
        }

        imu.setAccelRange(LSM6DSO32_ACCEL_RANGE_16_G);
        imu.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
        imu.setAccelDataRate(LSM6DS_RATE_3_33K_HZ);
        imu.setGyroDataRate(LSM6DS_RATE_3_33K_HZ);
    }

    float get_acceleration_x() {
        float output;
        imu.getEvent(&accel, &gyro, &temp);
        output = accel.acceleration.x;
        return output;
    }

    float get_acceleration_y() {
        float output;
        imu.getEvent(&accel, &gyro, &temp);
        output = accel.acceleration.y;
        return output;
    }

    float get_acceleration_z() {
        float output;
        imu.getEvent(&accel, &gyro, &temp);
        output = accel.acceleration.z;
        return output;
    }

    float get_gyro_x() {
        float output;
        imu.getEvent(&accel, &gyro, &temp);
        output = gyro.gyro.x;
        return output;
    }

    float get_gyro_y() {
        float output;
        imu.getEvent(&accel, &gyro, &temp);
        output = gyro.gyro.y;
        return output;
    }

    float get_gyro_z() {
        float output;
        imu.getEvent(&accel, &gyro, &temp);
        output = gyro.gyro.z;
        return output;
    }

};