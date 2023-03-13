#include "FlowState.h"

namespace StateMachine {

    FlowState flowState = FlowState();

    FlowState* getFlowState(){
        return &flowState;
    }

    FlowState::FlowState() {
        this->init();
    }

    /**
     * Prepare for start of flow
     */
    void FlowState::init() {
        Util::runMotors(0);
        lastPrint_ = 0;
        timeStarted_ = micros();
        pressureSetpoint_ = 0;
        angleSetpoint_ = 0;
        innerController_->reset();
        outerController_->reset();
    }

    /**
     * Perform single iteration of flow control loop 
     */
    void FlowState::update() {
        float motorAngle = HAL::getEncoderCount();

        HAL::readAllDucers();
        float UpstreamPsi = Ducers::readPressurantPT1();
        float DownstreamPsi = Ducers::readTankPT1();
        float rawDownstreamPsi = Ducers::readRawTankPT1();
        float rawUpstreamPsi = Ducers::readRawPressurantPT1();
        float filteredDownstreamPsi = Ducers::readFilteredTankPT1();
        float filteredUpstreamPsi = Ducers::readFilteredPressurantPT1();

        unsigned long flowTime = TimeUtil::timeInterval(timeStarted_, micros());
        float speed = 0;

        if (flowTime > Config::loxLead) {
            pressureSetpoint_ = FlowProfiles::flowPressureProfile(flowTime - Config::loxLead);

            //Use dynamic PID Constants
            Util::PidConstants dynamicPidConstants = Util::computeTankDynamicPidConstants(UpstreamPsi, DownstreamPsi, flowTime);
            outerController_->updateConstants(dynamicPidConstants.k_p, dynamicPidConstants.k_i, dynamicPidConstants.k_d);
            double feedforward = Util::compute_feedforward(pressureSetpoint_, UpstreamPsi, flowTime);

            //Compute Outer Pressure Control Loop
            angleSetpoint_ = outerController_->update(DownstreamPsi - pressureSetpoint_, feedforward);

            //Compute Inner PID Servo loop
            speed = innerController_->update(motorAngle - angleSetpoint_);

            Util::runMotors(speed);
            actuateMainValve(MAIN_VALVE_OPEN);
        } else {
            innerController_->reset();
            outerController_->reset();
        }

        //send data to AC
        if (TimeUtil::timeInterval(lastPrint_, micros()) > Config::telemetryInterval) {
            Packets::sendTelemetry(
                filteredUpstreamPsi,
                filteredDownstreamPsi,
                motorAngle,
                angleSetpoint_,
                pressureSetpoint_,
                speed,
                outerController_->getPTerm(),
                outerController_->getITerm(),
                outerController_->getDTerm()
            );
            lastPrint_ = micros();
        }

        if (flowTime > Config::getFlowDuration()) {
            enterIdleClosedState();
        }

        checkAbortPressure(DownstreamPsi, Config::abortPressureThresh);
    }

}