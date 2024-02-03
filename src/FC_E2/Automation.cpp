#include "Automation.h"

namespace Automation {

    //needs to handle launch, aborts, and autovent
    //and live video?

    //launch
    VehicleState vehicleState = IDLE;

    //autovent
    Comms::Packet autovent_config = {.id = FC_SEND_AUTOVENT, .len = 0};
    float lox_autoVentPressure = 600;
    float fuel_autoVentPressure = 600;
    bool lox_autoVentOpenState = false; // closed
    bool fuel_autoVentOpenState = false; // closed

    void onLaunch(Comms::Packet packet, uint8_t ip) {
        Mode systemMode = (Mode)packetGetUint8(&packet, 0); //now actually important. Dashboard must send right thing.
        if (systemMode = LAUNCH){
            vehicleState = FLIGHT;
        } else {
            vehicleState = FLOW;
        }
        //turn on live video?
    }

    void onAbort(Comms::Packet packet, uint8_t ip) {
        //EREG_Comms::forwardToOreg(packet);
        //EREG_Comms::forwardToFreg(packet);
        Mode systemMode = (Mode)packetGetUint8(&packet, 0);
        AbortReason abortReason = (AbortReason)packetGetUint8(&packet, 1);
        Serial.print("abort received: ");
        Serial.print(abortReason);
        Serial.print(" ");
        Serial.println(systemMode);

        // if we don't want aborts mid-flight, if vehicleMode == FLIGHT, return.
        //overpressure abort is the only one that could actually trigger.
        if (vehicleState == FLIGHT) {
            return; //LET IT BLOW LET IT BLOW LET IT BLOW 
        }

        switch(abortReason) {
            case TANK_OVERPRESSURE:
                //open lox and fuel gems
                AC::actuate(LOX_GEMS, AC::ON, 0);
                AC::actuate(FUEL_GEMS, AC::ON, 0);
                break;
            case ENGINE_OVERTEMP: // no tcs on flight
                AC::actuate(LOX_GEMS, AC::ON, 0);
                AC::actuate(FUEL_GEMS, AC::ON, 0);
                //close n2 flow and fill
                AC::actuate(N2_FLOW,AC::TIMED_RETRACT, 8000);
                break;
            case LC_UNDERTHRUST: // no lcs on flight
                AC::actuate(LOX_GEMS, AC::ON, 0);
                AC::actuate(FUEL_GEMS, AC::ON, 0);
                //close n2 flow and fill
                AC::actuate(N2_FLOW,AC::TIMED_RETRACT, 8000);
                break;
            case MANUAL_ABORT: //not on flight
                AC::actuate(LOX_GEMS, AC::ON, 0);
                AC::actuate(FUEL_GEMS, AC::ON, 0);
                //close n2 flow and fill
                AC::actuate(N2_FLOW,AC::TIMED_RETRACT, 8000);
                break;
            case IGNITER_NO_CONTINUITY:
            case BREAKWIRE_NO_CONTINUITY:
            case BREAKWIRE_NO_BURNT:
                AC::actuate(LOX_GEMS, AC::ON, 0);
                AC::actuate(FUEL_GEMS, AC::ON, 0);
                break;
        }
    }

    uint32_t task_sendAutoventConfig(){
        autovent_config.len = 0;
        Comms::packetAddFloat(&autovent_config, lox_autoVentPressure);
        Comms::packetAddFloat(&autovent_config, fuel_autoVentPressure);
        Comms::emitPacketToGS(&autovent_config);
        WiFiComms::emitPacketToGS(&autovent_config);
        Radio::forwardPacket(&autovent_config);
        return 1000*1000;
    }

    void setAutoVent(Comms::Packet packet, uint8_t ip){
        lox_autoVentPressure = packetGetFloat(&packet, 0);
        fuel_autoVentPressure = packetGetFloat(&packet, 4);
        Serial.println("lox auto vent pressure set to: " + String(lox_autoVentPressure));
        Serial.println("fuel auto vent pressure set to: " + String(fuel_autoVentPressure));

        //add to eeprom
        EEPROM.begin(2*sizeof(float));
        EEPROM.put(0, lox_autoVentPressure);
        EEPROM.put(sizeof(float), fuel_autoVentPressure);
        EEPROM.end();
    }

    void loxAutoVent(Comms::Packet packet, uint8_t ip){
        if (vehicleState == FLIGHT) {
            return;
        }
        float p1 = packetGetFloat(&packet, 0);
        float p2 = packetGetFloat(&packet, 4);

        if (p1 > lox_autoVentPressure || p2 > lox_autoVentPressure){
            Serial.println(AC::getActuatorState(LOX_GEMS));
            if (AC::getActuatorState(LOX_GEMS) == AC::OFF){
                lox_autoVentOpenState = true;
                AC::actuate(LOX_GEMS, AC::ON, 0);
            }
        } else {
        //close lox gems if open, and if autovent opened them. 
        // (if dashboard opened it, autoventstate is false and it won't close)
            if (lox_autoVentOpenState && AC::getActuatorState(LOX_GEMS) == AC::ON){
                lox_autoVentOpenState = false;
                AC::actuate(LOX_GEMS, AC::OFF, 0);

            }
        } 
    }

    void fuelAutoVent(Comms::Packet packet, uint8_t ip) {
        if (vehicleState == FLIGHT) {
            return;
        }
        float p1 = packetGetFloat(&packet, 0);
        float p2 = packetGetFloat(&packet, 4);
        Serial.println(p1);
        Serial.println(p2);
        Serial.println(fuel_autoVentPressure);
        if (p1 > fuel_autoVentPressure || p2 > fuel_autoVentPressure){
            if (AC::getActuatorState(FUEL_GEMS) == AC::OFF){
                fuel_autoVentOpenState = true;
                AC::actuate(FUEL_GEMS, AC::ON, 0);
            }
        } else {
        //close lox gems if open, and if autovent opened them. 
        // (if dashboard opened it, autoventstate is false and it won't close)
            if (AC::getActuatorState(FUEL_GEMS) == AC::ON && fuel_autoVentOpenState){
                fuel_autoVentOpenState = false;
                AC::actuate(FUEL_GEMS, AC::OFF, 0);
            }
        }
        
    }

    void init() {
        //launch automation
        Comms::registerCallback(STARTFLOW, onLaunch);

        //abort handlers
        Comms::registerCallback(ABORT, onAbort);
        //EREG_Comms::registerCallback(ABORT, onAbort);
        //what is desired behavior for abort in flight?

        //autovent handlers
        //EREG_Comms::registerCallback(FC_LOX_PRESSURE, loxAutoVent);
        //EREG_Comms::registerCallback(FC_FUEL_PRESSURE, fuelAutoVent);
        Comms::registerCallback(FC_LOX_PRESSURE, loxAutoVent);
        Comms::registerCallback(FC_LOX_PRESSURE, loxAutoVent);
        Comms::registerCallback(FC_SET_AUTOVENT, setAutoVent);
        //do we want autovent on during flight???
    }
}