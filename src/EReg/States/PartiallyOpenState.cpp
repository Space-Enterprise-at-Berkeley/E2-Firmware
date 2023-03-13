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

        HAL::readAllDucers();
        float upstreamPsi = Ducers::readPressurantPT1();
        float downstreamPsi = Ducers::readTankPT1();

        float rawDownstreamPsi = Ducers::readRawTankPT1();
        float rawUpstreamPsi = Ducers::readRawPressurantPT1();

        float filteredDownstreamPsi = Ducers::readFilteredTankPT1();
        float filteredUpstreamPsi = Ducers::readFilteredPressurantPT1();

        float speed = 0;

        //Compute Inner PID Servo loop
        speed = innerController_->update(motorAngle - angleSetpoint_);

        Util::runMotors(speed);

        //send data to AC
        if (TimeUtil::timeInterval(lastPrint_, micros()) > Config::telemetryInterval) {
            Packets::sendTelemetry(
                filteredUpstreamPsi,
                filteredDownstreamPsi,
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