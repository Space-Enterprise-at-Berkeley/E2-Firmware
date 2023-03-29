#pragma once

#include "Actuators.h"
#include "Ereg.h"

#include <Arduino.h>
#include <stdlib.h>
#include <AC/Comms.h>

namespace Automation {

    const float autoVentUpperThreshold = 750; //TODO actual value 650 - 620
    const float autoVentLowerThreshold = 700;

    const float igniterTriggerThreshold = 0.06;
    const float breakWireThreshold = 20;
    const float twoWayCurrentThreshold = 0.01; //TODO find out why this is not working

    const long burnTime = 18 * 1e6;

    extern bool igniterEnabled;
    extern bool breakwireEnabled;
    extern bool igniterTriggered;

    extern Comms::Packet lcAbortPacket;

    extern Task *flowTask; // perform burn
    extern Task *abortFlowTask;
    extern Task *autoventFuelTask;
    extern Task *autoventLoxTask;
    extern Task *checkForTCAbortTask;
    extern Task *checkForLCAbortTask;

    void initAutomation(Task *flowTask, Task *abortFlowTask, Task *autoventFuelTask, Task *autoventLoxTask);
    void beginFlow(Comms::Packet packet, uint8_t ip);
    uint32_t flow();
    void beginManualAbortFlow(Comms::Packet packet, uint8_t ip);
    void beginAbortFlow();
    uint32_t abortFlow();
    uint32_t autoventFuelGemValveTask();
    uint32_t autoventLoxGemValveTask();
    uint32_t checkIgniter();
    void checkForLCAbort(Comms::Packet packet, uint8_t ip);
    void checkForTCAbort(Comms::Packet packet, uint8_t ip);

    void sendFlowStatus(uint8_t status);

};

#define STATE_FLOW_PREPARATION 0
#define STATE_ACTIVATE_IGNITER 1
#define STATE_BREAKWIRE_FAIL_CONTINUITY_ABORT 2
#define STATE_IGNITER_ACTIVATED 3
#define STATE_ARMED_VALVES 4
#define STATE_BREAKWIRE_FAIL_DISCONNECT_ABORT 5
#define STATE_START_FLOW 6
#define STATE_ARMING_VALVE_FAIL_CURRENT 7
#define STATE_DISABLE_ARMING_VALVE 8
#define STATE_END_FLOW 9
#define STATE_MANUAL_ABORT 10
#define STATE_ABORT_END 11
#define STATE_EREG_ABORT 12
#define STATE_LC_ABORT 13
#define STATE_TC_ABORT 14


