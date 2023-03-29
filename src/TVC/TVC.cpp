#include "TVC.h"
#include "EEPROM.h"
#include "HAL.h"

namespace TVC {

    // define update period
    uint32_t tvcUpdatePeriod = 50 * 1000;
    int x_motor_ticksp = 0;
    int y_motor_ticksp = 0;
    float x_p = 0.3;
    float x_i = 0;
    float x_d = 0;
    float y_p = 0.3;
    float y_i = 0;
    float y_d = 0;
    int MID_SPD = 307;
    //(4096 * (1500 / 200000)))
    int MAX_SPD = 20;
    int MIN_SPD = -20;
    int INNER_BUFFER_SIZE = 2;
    int speed_x = 10;
    int speed_y = 10;

    int idx = 0;
    int idy = 150;

    int tvcState = 0; //0: constant power move, 1: pid control
    bool circleEnabled = false;

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
        x_Controller.reset();
        y_Controller.reset();
        delay(3000); 
    }


    void definePosition(Comms::Packet defPos, uint8_t id) {
        // Serial.println("Packet");
        float x_pos = packetGetFloat(&defPos, 0);
        float y_pos = packetGetFloat(&defPos, 4);

        x_motor_ticksp = (int)x_pos;
        y_motor_ticksp = (int)y_pos;

        // Serial.println("Received command to define encoder position: (" + String(x_pos) + ", " + String(y_pos) + ")");
        // Serial.println("X setpoint is: " + String(x_motor_ticksp) + ", Y setpoint is: " + String(y_motor_ticksp));
    }


    // uint32_t updatePID() {

    //     // Serial.println("PID time");

    //     speed_x = x_Controller.update(x_motor_ticksp - HAL::getEncoderCount_0()) + MID_SPD;
    //     // speed_x = -(x_motor_ticksp - HAL::encoderTicks)*x_p + MID_SPD;

    //     // Serial.print("x Speed: ");
    //     // Serial.println(speed_x);
    //     // Serial.print("Encoder count x: ");
    //     // Serial.println(HAL::getEncoderCount_0());
    //     // Serial.print("x setpoint - encoder count x: ");
    //     // Serial.println(x_motor_ticksp - HAL::getEncoderCount_0());

    //     speed_y = y_Controller.update(y_motor_ticksp - HAL::getEncoderCount_1()) + MID_SPD;

    //     // Serial.print("y Speed: ");
    //     // Serial.println(speed_y);
    //     // Serial.print("Encoder count y: ");
    //     // Serial.println(HAL::getEncoderCount_1());
    //     // Serial.print("y setpoint - encoder count y: ");
    //     // Serial.println(y_motor_ticksp - HAL::getEncoderCount_1());

    //     // Serial.println("X encoder count: " + String(HAL::getEncoderCount_0()));
    //     // Serial.println("Y encoder count: " + String(HAL::getEncoderCount_1()));

    //     Comms::Packet tmp = {.id=12};
    //     Comms::packetAddUint32(&tmp, HAL::getEncoderCount_0());
    //     Comms::packetAddUint32(&tmp, HAL::getEncoderCount_1());
    //     Comms::emitPacketToGS(&tmp);

    //     ledcWrite(0, speed_x);
    //     ledcWrite(1, speed_y);
        
    //     // Serial.println("Running PWM");
    //     return tvcUpdatePeriod;
    // }

uint32_t moveTVC() {

    if (tvcState) { // moving with PID
        // Serial.println("PID time");

        if (circleEnabled) { 
            x_motor_ticksp = circleTicks[idx];
            y_motor_ticksp = circleTicks[idy];

            idx += 3;
            idy += 3;
            if(idx >= sizeof(circleTicks)) idx = 0;
            if(idy >= sizeof(circleticks)) idy = 0;
        }

        speed_x = x_Controller.update(x_motor_ticksp - HAL::getEncoderCount_0()) + MID_SPD;
        // speed_x = -(x_motor_ticksp - HAL::encoderTicks)*x_p + MID_SPD;

        // Serial.print("x Speed: ");
        // Serial.println(speed_x);
        // Serial.print("Encoder count x: ");
        // Serial.println(HAL::getEncoderCount_0());
        // Serial.print("x setpoint - encoder count x: ");
        // Serial.println(x_motor_ticksp - HAL::getEncoderCount_0());

        speed_y = y_Controller.update(y_motor_ticksp - HAL::getEncoderCount_1()) + MID_SPD;

        // Serial.print("y Speed: ");
        // Serial.println(speed_y);
        // Serial.print("Encoder count y: ");
        // Serial.println(HAL::getEncoderCount_1());
        // Serial.print("y setpoint - encoder count y: ");
        // Serial.println(y_motor_ticksp - HAL::getEncoderCount_1());

        // Serial.println("X encoder count: " + String(HAL::getEncoderCount_0()));
        // Serial.println("Y encoder count: " + String(HAL::getEncoderCount_1()));
    } 

        Comms::Packet tmp = {.id=12};
        Comms::packetAddUint32(&tmp, HAL::getEncoderCount_0());
        Comms::packetAddUint32(&tmp, HAL::getEncoderCount_1());
        Comms::emitPacketToGS(&tmp);

        ledcWrite(0, speed_x);
        ledcWrite(1, speed_y);
        
        // Serial.println("Running PWM");

        return tvcUpdatePeriod;
    }

    void setMode(int mode) { 
        tvcState = mode;
        // mode 0: manual, 1: PID
    }

    void enableCircle(bool en) { 
        circleEnabled = en;
        // add delay after? 
    }

    void setXSpeed(int spdx) { 
        speed_x = spdx;
    }

    void setYSpeed(int spdy) { 
        speed_y = spdy;
    }

    // possible states: move at constant power for x time; move to setpoints (pid)

    
}
