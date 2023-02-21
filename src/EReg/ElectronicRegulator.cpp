#include <Arduino.h>
#include "HAL.h"
#include "Util.h"
#include "EspComms.h"
#include "Config.h"
#include "StateMachine.h"
#include "Packets.h"
#include "Ducers.h"

StateMachine::FlowState *flowState = StateMachine::getFlowState();
StateMachine::IdleClosedState *idleClosedState = StateMachine::getIdleClosedState();
StateMachine::PartiallyOpenState *partiallyOpenState = StateMachine::getPartiallyOpenState();
StateMachine::DiagnosticState *diagnosticState = StateMachine::getDiagnosticState();
StateMachine::PressurizeState *pressurizeState = StateMachine::getPressurizeState();

void zero() { 
    DEBUGLN("starting zero command");
    Util::runMotors(-50);
    delay(2000);
    Util::runMotors(0); 
    // zero encoder value (so encoder readings range from -x (open) to 0 (closed))
    delay(400);
    HAL::setEncoderCount(-20);
    DEBUG("encoder position after zero: ");
    DEBUGLN(HAL::getEncoderCount());
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
    delay(3000);
    Serial.begin(115200);
    Serial.printf("micros: %d\n", micros());
    Serial.printf("hi!!\n");
    if (HAL::init() == -1) {
        DEBUGF("HAL initialization failed\n");
    } else {
        DEBUGF("HAL initialization success!\n");
    }
    Comms::init(HAL::ETH_CS, HAL::ETH_MISO, HAL::ETH_MOSI, HAL::ETH_SCLK);
    Serial.printf("Comms init done!\n");
    StateMachine::enterIdleClosedState();
    zero(); 
    Comms::registerCallback(200, flow);
    Comms::registerCallback(201, stopFlow);
    Comms::registerCallback(202, partialOpen);
    Comms::registerCallback(203, pressurize);
    Comms::registerCallback(204, runDiagnostics);
    Comms::registerCallback(205, zero);
    // Comms::registerCallback(206, actuateMainValve);
    
    Packets::sendConfig();
}

void loop() {
    Comms::processWaitingPackets();
    Util::checkMotorDriverHealth();
    switch (StateMachine::getCurrentState()) {
        case StateMachine::IDLE_CLOSED:
        idleClosedState->update();
        break;
        
        case StateMachine::PARTIAL_OPEN:
        partiallyOpenState->update();
        break;

        case StateMachine::PRESSURIZE:
        pressurizeState->update();
        break;

        case StateMachine::FLOW:
        flowState->update();
        break;

        case StateMachine::DIAGNOSTIC:
        diagnosticState->update();
        break;
    };
}

