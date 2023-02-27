//Fuel EReg Config 
#pragma once
#include "../FlowProfiles.h"

namespace Config {

    #define MAX_ANGLE 260                                                  
    #define MIN_ANGLE 0
    #define ANTIWINDUP_RANGE_LOWER 43
    #define ANTIWINDUP_RANGE_UPPER 188

    #define OUTER_BUFFER_SIZE 3

    // Controller Constants
    // const double p_outer_nominal = 1.8, i_outer_nominal = 0.30e-6, d_outer_nominal = 0.24; // nominal is 4000 -> 500 psi flow
    // const double p_outer_nominal = 1.8, i_outer_nominal = 0.084e-6, d_outer_nominal = 0.067; // nominal is 4000 -> 500 psi flow
    const double p_outer_nominal = 1.8, i_outer_nominal = 0.084e-6, d_outer_nominal = 0; // nominal is 4000 -> 500 psi flow
    // const double p_inner = 6, i_inner = 3.5e-6, d_inner = 0.10;
    const double p_inner = 0.6, i_inner = 0, d_inner = .15;


    // Flow Parameters
    const float pressureSetpoint = 550; // hotfire 7 was 495
    const unsigned long loxLead = 105UL * 1000UL; //time in microseconds

    // Diagnostic configs
    const int servoTestPoints = 5;
    const float servoTravelInterval = 30; // encoder counts
    const unsigned long servoSettleTime =  200UL * 1000UL; // micros
    const float stopDiagnosticPressureThresh = 200; // diagnostic terminates if either tank exceeds this
    const float diagnosticSpeed = 200;
}
