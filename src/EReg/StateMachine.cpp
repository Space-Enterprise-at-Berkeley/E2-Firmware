#include "StateMachine.h"

namespace StateMachine {

    State currentState = IDLE_CLOSED;
    ValveAction currentMainValveState = MAIN_VALVE_CLOSE;
    const int bufferSize = 100;
    float overpressureBuffer[bufferSize];
    uint8_t opBuffCnt;

    void enterFlowState() {
        if (currentState == IDLE_CLOSED) {
            currentState = FLOW;
            getFlowState()->init();
        } else {
            // Illegal state transition
            Packets::sendStateTransitionError(0);
        }
    }

    void enterIdleClosedState() {
        currentState = IDLE_CLOSED;
        getIdleClosedState()->init();
    }

    void enterPartialOpenState(float angle) {
        if (currentState == IDLE_CLOSED || currentState == PARTIAL_OPEN) {
            if (angle > (MIN_ANGLE - 100) && angle < (MAX_ANGLE + 100)) {
                currentState = PARTIAL_OPEN;
                getPartiallyOpenState()->init(angle);
            } else {
                // Illegal parameters
                Packets::sendStateTransitionError(1);
            }
        } else {
            // Illegal state transition
            Packets::sendStateTransitionError(2);
        }
    }

    void enterDiagnosticState() {
        float pressure = HAL::readUpstreamPT();
        if (pressure > 200) {
            Packets::sendStateTransitionError(3);
        } else {
            if (currentState == IDLE_CLOSED) {
                currentState = DIAGNOSTIC;
                getDiagnosticState()->init();
            } else {
                // Illegal state transition
                Packets::sendStateTransitionError(3);
            }
        }
    }

    void enterPressurizeState() {

        if (currentState == IDLE_CLOSED) {
            currentState = PRESSURIZE;
            getPressurizeState()->init();
        } else {
            // Illegal state transition
            Packets::sendStateTransitionError(4);
        }
    }



    State getCurrentState() {
        return currentState;
    }

    /**
     * Helper function to actuate main valve. DO NOT have outside code calling this function - 
     * This function does not validate the current state
     * @param action Desired valve state
     */
    void actuateMainValve(ValveAction action) {
        return; //no more main valve!
    }

    void checkAbortPressure(float currentPressure, float abortPressure) {

        // overpressureBuffer[opBuffCnt] = currentPressure;
        // opBuffCnt = (opBuffCnt + 1) % bufferSize;

        // float total;

        // for(uint8_t i = 0; i < bufferSize; i++) {
        //     total += overpressureBuffer[i];
        // }

        // total /= bufferSize;
        // return; //FIX THIS
        if (currentPressure > abortPressure) {
            // Packets::sendFlowState(0);
            Packets::broadcastAbort();
            StateMachine::enterIdleClosedState();
            Serial.printf("overpressure abort %f\n", currentPressure);
        }
    }

}
