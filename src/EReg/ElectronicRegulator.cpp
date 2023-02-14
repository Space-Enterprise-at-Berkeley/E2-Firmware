#include <Arduino.h>
#include "HAL.h"
#include "Util.h"
#include "Comms.h"
#include "Config.h"
#include "StateMachine.h"
#include "Packets.h"
#include "Ducers.h"

StateMachine::FlowState *flowState = StateMachine::getFlowState();
StateMachine::IdleClosedState *idleClosedState = StateMachine::getIdleClosedState();
StateMachine::PartiallyOpenState *partiallyOpenState = StateMachine::getPartiallyOpenState();
StateMachine::DiagnosticState *diagnosticState = StateMachine::getDiagnosticState();
StateMachine::PressurizeState *pressurizeState = StateMachine::getPressurizeState();
StateMachine::InjectorFlowState *injectorFlowState = StateMachine::getInjectorFlowState();

void zero() { 
    DEBUGLN("starting zero command");
    Util::runMotors(-50);
    delay(2000);
    Util::runMotors(0);
    // zero encoder value (so encoder readings range from -x (open) to 0 (closed))
    delay(400);
    HAL::encoder.setCount(-20);
    DEBUG("encoder position after zero: ");
    DEBUGLN(HAL::encoder.getCount());
}

void zero(Comms::Packet packet, uint8_t ip) {
    zero();
}

void flow(Comms::Packet packet, uint8_t ip) {
    StateMachine::enterFlowState();
}

void stopFlow(Comms::Packet packet, uint8_t ip) {
    StateMachine::enterIdleClosedState();
}

void partialOpen(Comms::Packet packet, uint8_t ip) {
    StateMachine::enterPartialOpenState(Comms::packetGetFloat(&packet, 0));
}

void runDiagnostics(Comms::Packet packet, uint8_t ip) {
    StateMachine::enterDiagnosticState();
}

void pressurize(Comms::Packet packet, uint8_t ip) {
    StateMachine::enterPressurizeState();
}

void actuateMainValve(Comms::Packet packet, uint8_t ip) {
    StateMachine::enterMainValveState(Comms::packetGetUint8(&packet, 0));
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    Serial.printf("hi!!\n");
    delay(5000);

    // if (HAL::init() == -1) {
    //     DEBUGF("HAL initialization failed\n");
    //     while (true) {1;}
    // } else {
    //     DEBUGF("HAL initialization success!\n");
    // }
    // Serial.printf("hal init\n");
    // Comms::initComms();
    // StateMachine::enterIdleClosedState();
    // zero(); 
    // Comms::registerCallback(200, flow);
    // Comms::registerCallback(201, stopFlow);
    // Comms::registerCallback(202, partialOpen);
    // Comms::registerCallback(203, pressurize);
    // Comms::registerCallback(204, runDiagnostics);
    // Comms::registerCallback(205, zero);
    // Comms::registerCallback(206, actuateMainValve);
    
    // Packets::sendConfig();
}

void loop() {
    // Comms::processWaitingPackets();
    // Util::checkMotorDriverHealth();
    // switch (StateMachine::getCurrentState()) {
    //     case StateMachine::IDLE_CLOSED:
    //     idleClosedState->update();
    //     break;
        
    //     case StateMachine::PARTIAL_OPEN:
    //     partiallyOpenState->update();
    //     break;

    //     case StateMachine::PRESSURIZE:
    //     pressurizeState->update();
    //     break;

    //     case StateMachine::FLOW:
    //     flowState->update();
    //     break;

    //     case StateMachine::DIAGNOSTIC:
    //     diagnosticState->update();
    //     break;
    // };
}

