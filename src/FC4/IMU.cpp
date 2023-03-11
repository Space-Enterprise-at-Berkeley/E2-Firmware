#include "IMU.h"


namespace IMU {

    
    
    void initIMU() { 

    }

    uint32_t imuSample() { 

    }

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

};