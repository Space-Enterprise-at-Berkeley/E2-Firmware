#include "ChannelMonitor.h"
#include <Arduino.h>
#include <EspComms.h>

// Channel Monitor monitors continuity and currents for each actuator channel
namespace ChannelMonitor {

// sel0 through sel2 are the MUX select pins, currpins and contpin are th pins via which readings can be taken via analog read
uint8_t sel0, sel1, sel2, mux1out, mux2out;

// update period for the current/continuity task
uint32_t cmUpdatePeriod;

// publically accessible via getters, last updated values held here
float mux1Data[8] = {};
float mux2Data[8] = {};
float currents[8] = {};
float continuities[8] = {};
float breakwires[2] = {};

// voltage threshold for continuity to be detected
float CONT_THRESHOLD = 0.5;

// Minimum current draw to determine if a motor is running or not
float RUNNING_THRESH = 0.1;


// sets up mux's
void init(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t mux1out_pin, uint8_t mux2out_pin){
    sel0 = s0;
    sel1 = s1;
    sel2 = s2;
    mux1out = mux1out_pin;
    mux2out = mux2out_pin;

    // every 100 ms
    cmUpdatePeriod = 1000 * 100;

    pinMode(sel0, OUTPUT);
    pinMode(sel1, OUTPUT);
    pinMode(sel2, OUTPUT);
    pinMode(mux1out, INPUT);
    pinMode(mux2out, INPUT);
}

// converts ADC current counts to current in amps
// Current is read as follows: the actual current / 5000 is sourced across a 4.53k resistor yielding a read voltage
// Read out of 4096 ADC counts where 4096 = 3.3V
// TODO characterise the current to ADC counts relationship better (via emperical testing with the E-Load)
float adcToCurrent(uint16_t counts) {
    return (counts / 4096.0) * 4530;
}

float adcToCont(uint16_t counts) {
    return (counts / 4096.0) * 3.3;
}

// reads currents and continuity, reports them via packets and by setting the above arrays
uint32_t task_readChannels() {


    Comms::Packet contPacket = {.id = FC_ACT_CONT};
    Comms::Packet currPacket = {.id = FC_ACT_CURR};
    Comms::Packet breakwirePacket = {.id = FC_BREAKWIRE};

    // iterate through MUX channels
    for (int i = 0; i < 8; i ++){
        digitalWrite(sel0, i & 0x01);
        digitalWrite(sel1, (i >> 1) & 0x01);
        digitalWrite(sel2, (i >> 2) & 0x01);
        delayMicroseconds(1);

        // read raw current and continuity voltages in ADC counts

        uint16_t mux1Val = analogRead(mux1out);
        uint16_t mux2Val = analogRead(mux2out);

        //Serial.println(mux1Val);
        mux1Data[i] = mux1Val;
        mux2Data[i] = mux2Val;
    } 
    
    // MAPPING (kinda sad that this is how it is but I low key think this is the easiest way)
    currents[0] = adcToCurrent(mux2Data[4]);
    currents[1] = adcToCurrent(mux2Data[0]);
    currents[2] = adcToCurrent(mux2Data[5]);
    currents[3] = adcToCurrent(mux1Data[1]);
    currents[4] = adcToCurrent(mux1Data[3]);

    continuities[0] = adcToCont(mux1Data[2]);
    continuities[1] = adcToCont(mux2Data[3]);
    continuities[2] = adcToCont(mux2Data[6]);
    continuities[3] = adcToCont(mux1Data[0]);
    continuities[4] = adcToCont(mux1Data[5]);

    breakwires[0] = adcToCont(mux1Data[4]);
    breakwires[1] = adcToCont(mux1Data[6]);

    for (int i = 0; i < 5; i++) {
        Comms::packetAddFloat(&currPacket, currents[i]);
    }

    for (int i = 0; i < 5; i++) {
        Comms::packetAddFloat(&contPacket, continuities[i]);
    }

    Comms::packetAddFloat(&breakwirePacket, breakwires[0]);
    Comms::packetAddFloat(&breakwirePacket, breakwires[1]);
    //Serial.println(breakwires[0]);
    //Serial.println(breakwires[1]);



    Comms::emitPacketToGS(&currPacket);
    WiFiComms::emitPacketToGS(&currPacket);
    Radio::forwardPacket(&currPacket);
    Comms::emitPacketToGS(&contPacket);
    WiFiComms::emitPacketToGS(&contPacket);
    Radio::forwardPacket(&contPacket);
    Comms::emitPacketToGS(&breakwirePacket);
    WiFiComms::emitPacketToGS(&breakwirePacket);
    Radio::forwardPacket(&breakwirePacket);

    return cmUpdatePeriod;
}


// getters
float* getCurrents() {
    return currents;
}

float* getContinuities() {
    return continuities;
}

bool isChannelContinuous(uint8_t channel) {
    return continuities[channel] > CONT_THRESHOLD;
}

}