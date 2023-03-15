#include "TVC.h"
#include "EEPROM.h"
#include "HAL.h"

// #define X_PWM_PIN 6
// #define Y_PWM_PIN 7

namespace TVC {

    // define update period
    uint32_t tvcUpdatePeriod = 50 * 1000;
    int x_motor_ticksp = 0;
    int y_motor_ticksp = 0;
    float x_p = 0.1;
    float x_i = 0;
    float x_d = 0;
    float y_p = 0;
    float y_i = 0;
    float y_d = 0;
    int MID_SPD = 307;
    int MAX_SPD = 20; //(4096 * (1500 / 200000)))
    int MIN_SPD = -20;
    int INNER_BUFFER_SIZE = 2;
    // int x_tickSetpoint = 0;
    int y_tickSetpoint = 0;
    int speed_x;
    int speed_y;

    // define class variables    
    PIDController x_Controller(
        x_p, x_i, x_d, 
        MIN_SPD, MAX_SPD, 
        PIDController::standard, 
        INNER_BUFFER_SIZE);

    PIDController y_Controller(
        y_p, y_i, y_d, 
        MIN_SPD, MAX_SPD, 
        PIDController::standard, 
        INNER_BUFFER_SIZE);

    void init() {
        // pinMode(OUTPUT, X_PWM_PIN);   

        // ledcSetup(0, 50, 12);
        // ledcAttachPin(HAL::x_pwm, 0);
        // ledcWrite(0, 307);

        // ledcSetup(1, 50, 12);
        // ledcAttachPin(HAL::y_pwm, 1);
        // ledcWrite(1, 307);

        // analogWriteFrequency(X_PWM_PIN, 50);
        // analogWriteResolution(12);  
        // analogWrite(X_PWM_PIN, 307); 
        x_Controller.reset();
        y_Controller.reset();
        delay(3000); 
    }

    uint32_t updatePID() {
        // speed_x = x_Controller.update(x_motor_ticksp - HAL::encoderTicks) + MID_SPD;
        speed_x = -(x_motor_ticksp - HAL::encoderTicks)*x_p + MID_SPD;
        Serial.println(speed_x);
        Serial.println(x_motor_ticksp - HAL::encoderTicks);
        speed_y = y_Controller.update(y_motor_ticksp - y_tickSetpoint) + MID_SPD;

        // analogWrite(X_PWM_PIN, speed_x);
        // analogWrite(Y_PWM_PIN, speed_y);
        ledcWrite(0, speed_x);
        ledcWrite(1, speed_y);

        // analogWrite(X_PWM_PIN, 655);
        Serial.println("Running PWM");
        return tvcUpdatePeriod;
    }
}
