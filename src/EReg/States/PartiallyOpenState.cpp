#include "PartiallyOpenState.h"

namespace StateMachine {
    PartiallyOpenState partiallyOpenState = PartiallyOpenState();

    PartiallyOpenState* getPartiallyOpenState() {
        return &partiallyOpenState;
    }

    
    PartiallyOpenState::PartiallyOpenState() {
        // this->init(0);
    }

    /**
     * Prepare controllers to enter angle servo mode (partially open ereg)
     */
    void PartiallyOpenState::init(float angleSetpoint) {
        Util::runMotors(0);
        lastPrint_ = 0;
        angleSetpoint_ = angleSetpoint;
        innerController_->reset();
    }

    /**
     * Perform single iteration of valve angle servo loop
     */
    void PartiallyOpenState::update() {
        float motorAngle = HAL::getEncoderCount();

        float upstreamPsi = HAL::readUpstreamPT();
        float downstreamPsi = HAL::readDownstreamPT();

        float speed = 0;

        //Compute Inner PID Servo loop
        speed = innerController_->update(motorAngle - angleSetpoint_);

        Util::runMotors(speed);

        //send data to AC
        if (TimeUtil::timeInterval(lastPrint_, micros()) > Config::telemetryInterval) {
            Packets::sendTelemetry(
                upstreamPsi,
                downstreamPsi,
                motorAngle,
                angleSetpoint_,
                0,
                speed,
                0,
                0,
                0
            );
            lastPrint_ = micros();
        }

        checkAbortPressure(downstreamPsi, Config::abortPressureThresh);
    }

}