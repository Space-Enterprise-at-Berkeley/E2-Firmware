#include "TVC.h"
#include "EEPROM.h"
#include "HAL.h"

// #define X_PWM_PIN 6
// #define Y_PWM_PIN 7

namespace TVC {

    // define update period
    uint32_t tvcUpdatePeriod = 50 * 1000;
    int x_motor_ticksp = 10000;
    int y_motor_ticksp = 0;
    float x_p = 0.1;
    float x_i = 0;
    float x_d = 0;
    float y_p = 0;
    float y_i = 0;
    float y_d = 0;
    int MID_SPD = 307;
    // int MAX_SPD = 20; //(4096 * (1500 / 200000)))
    // int MIN_SPD = -20;
    int MAX_SPD = 50;
    int MIN_SPD = -50;
    int INNER_BUFFER_SIZE = 2;
    int x_tickSetpoint = 0;
    int y_tickSetpoint = 0;
    int speed_x = 40;
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

        x_Controller.reset();
        y_Controller.reset();
        delay(3000); 
    }

    void definePosition(Comms::Packet defPos, uint8_t id) {
        float x_pos = packetGetFloat(&defPos, 0);
        float y_pos = packetGetFloat(&defPos, 4);

        x_motor_ticksp = (int)x_pos;
        y_motor_ticksp = (int)y_pos;

        Serial.println("Received command to define encoder position: (" + String(x_pos) + ", " + String(y_pos) + ")");
    }

    uint32_t updatePID() {

        Serial.println("PID time");

        speed_x = x_Controller.update(x_motor_ticksp - HAL::getEncoderCount()) + MID_SPD;
        // speed_x = -(x_motor_ticksp - HAL::encoderTicks)*x_p + MID_SPD;

        Serial.print("Speed: ");
        Serial.println(speed_x);
        Serial.print("Encoder count: ");
        Serial.println(HAL::getEncoderCount());
        Serial.print("setpoint - encoder count: ");
        Serial.println(x_motor_ticksp - HAL::getEncoderCount());
        // speed_y = y_Controller.update(y_motor_ticksp - y_tickSetpoint) + MID_SPD;

        Comms::Packet tmp = {.id=12};
        Comms::packetAddUint32(&tmp, HAL::getEncoderCount());
        Comms::emitPacketToGS(&tmp);

        ledcWrite(0, speed_x);
        // ledcWrite(1, speed_y);

        // analogWrite(X_PWM_PIN, 655);
        Serial.println("Running PWM");
        return tvcUpdatePeriod;
    }
}
