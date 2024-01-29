#include "HAL.h"
#include <EspComms.h>
#include <Arduino.h>
#include "Barometer.h"
#include "IMU.h"
#include "GPS.h"
#include <Common.h>

namespace FlightStatus {

    // todo: everything for apogee
    // kalman filter params
    // handle read sensors
    // handle send flight sensor packets
    
    uint8_t launched = 0;
    uint8_t burnout = 0;
    uint8_t apogee = 0;
    uint8_t main_parachute = 0;
    uint8_t deploy_vel = 0;

    uint32_t updateFlight();

    

};