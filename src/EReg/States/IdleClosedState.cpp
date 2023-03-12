#include "IdleClosedState.h"

namespace StateMachine {

    IdleClosedState idleClosedState = IdleClosedState();

    IdleClosedState* getIdleClosedState() {
        return &idleClosedState;
    }

    IdleClosedState::IdleClosedState() {
        this->init();
    }

    /**
     * Initialization for closing ereg valve
     * Note that this call begins valve closing action immediately, without waiting for update()
     */
    void IdleClosedState::init() {
        timeStarted_ = millis();
        lastPrint_ = 0;
        Util::runMotors(closeSpeed_);
    }

    /**
     * IdleClosed update function
     * If sufficient time has passed, turn off motors
     * Transmit telemetry
     */
    void IdleClosedState::update() {
        float motorAngle = HAL::getEncoderCount();

        HAL::readAllDucers();
        float upstreamPsi = Ducers::readPressurantPT1();
        float downstreamPsi = Ducers::readTankPT1();
        float rawDownstreamPsi = Ducers::readRawTankPT1();
        float rawUpstreamPsi = Ducers::readRawPressurantPT1();
        // Serial.printf("%.2f\n", downstreamPsi);

        //Compute Inner PID Servo loop
        float speed = 0;
        if (TimeUtil::timeInterval(timeStarted_, millis()) <= runTime_) {
            speed = closeSpeed_;
        }

        Util::runMotors(speed);

        // send data to AC
        if (TimeUtil::timeInterval(lastPrint_, micros()) > Config::telemetryIntervalIdle) {
            Packets::sendTelemetry(
                rawUpstreamPsi,
                rawDownstreamPsi,
                motorAngle,
                0,
                0,
                speed,
                0,
                0,
                0
            );
            
            lastPrint_ = micros();
            // Serial.printf("downstream pt: %f\n", downstreamPsi);
        }
    }

}