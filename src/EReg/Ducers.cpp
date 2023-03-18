#include "Ducers.h"

namespace Ducers {

    //set on call of HAL::readAllDucers(). 
    bool persistentCal = true;

    float data[4];
    float offset[4];
    float multiplier[4];

    // //0
    // //float _upstreamPT1_offset = 0;
    // //float _upstreamPT1_multiplier = 1;

    // //1
    // float _downstreamPT1_offset = 0;
    // float _downstreamPT1_multiplier = 1;

    // //2
    // float _upstreamPT2_offset = 0;
    // float _upstreamPT2_multiplier = 1;


    // //3
    // float _downstreamPT2_offset = 0;
    // float _downstreamPT2_multiplier = 1;

    float _upstreamPT1;
    float _downstreamPT1;
    float _upstreamPT2;
    float _downstreamPT2;

    Buffer* upstreamPT1Buff;
    Buffer* downstreamPT1Buff;
    Buffer* upstreamPT2Buff;
    Buffer* downstreamPT2Buff;
    
    void setDownstreamPT1(float downstreamPT1) {
        _downstreamPT1 = downstreamPT1;
        downstreamPT1Buff->insert(millis(), downstreamPT1);
    }
    void setDownstreamPT2(float downstreamPT2) {
        _downstreamPT2 = downstreamPT2;
        downstreamPT2Buff->insert(millis(), downstreamPT2);
    }
    void setUpstreamPT1(float upstreamPT1) {
        _upstreamPT1 = upstreamPT1;
        upstreamPT1Buff->insert(millis(), upstreamPT1);
    }
    void setUpstreamPT2(float upstreamPT2) {
        _upstreamPT2 = upstreamPT2;
        upstreamPT2Buff->insert(millis(), upstreamPT2);
    }

    void zeroChannel(uint8_t channel){
        float value;

        if(channel == 0){
            value = readFilteredPressurantPT1();
        }
        else if(channel == 1){
            value = readFilteredTankPT1();
        }
        else if(channel == 2){
            value = readFilteredPressurantPT2();
        }
        else if(channel == 3){
            value = readFilteredTankPT2();
        }
        
        offset[channel] = -value + offset[channel];
        Serial.println("zeroed channel " + String(channel) + " to " + String(offset[channel]));
        if (persistentCal){
            EEPROM.begin(8*sizeof(float));
            EEPROM.put(channel*sizeof(float),offset[channel]);
            EEPROM.end();
        }
    }

    void calChannel(uint8_t channel, float inputvalue){
        float value;

        if(channel == 0){
            value = readFilteredPressurantPT1();
        }
        else if(channel == 1){
            value = readFilteredTankPT1();
        }
        else if(channel == 2){
            value = readFilteredPressurantPT2();
        }
        else if(channel == 3){
            value = readFilteredTankPT2();
        }
        multiplier[channel] *= inputvalue / value;
        Serial.println("calibrated channel multiplier" + String(channel) + " to " + String(multiplier[channel]));
        if (persistentCal){
            EEPROM.begin(8*sizeof(float));
            EEPROM.put((channel+4)*sizeof(float),multiplier[channel]);
            EEPROM.end();
        }
    }

    void onZeroCommand(Comms::Packet packet, uint8_t ip){
        uint8_t channel = Comms::packetGetUint8(&packet, 0);
        zeroChannel(channel);
        return;
    }

    void onCalCommand(Comms::Packet packet, uint8_t ip){
        uint8_t channel = Comms::packetGetUint8(&packet, 0);
        float value = Comms::packetGetFloat(&packet, 2);
        calChannel(channel, value);
        return;
    }


    void initPTs() {
            upstreamPT1Buff = new Buffer(Config::PTFilterBufferSize);
            downstreamPT1Buff = new Buffer(Config::PTFilterBufferSize);
            upstreamPT2Buff = new Buffer(Config::PTFilterBufferSize);
            downstreamPT2Buff = new Buffer(Config::PTFilterBufferSize);
            upstreamPT1Buff->clear();
            downstreamPT1Buff->clear();
            upstreamPT2Buff->clear();
            downstreamPT2Buff->clear();

            Comms::registerCallback(100, onZeroCommand);
            Comms::registerCallback(101, onCalCommand);

            if (persistentCal){
            EEPROM.begin(8*sizeof(float));
            for (int i = 0; i < 4; i++){
                EEPROM.get(i*sizeof(float),offset[i]);
                if (isnan(offset[i])){
                    offset[i] = 0;
                }
            }
            for (int i = 0; i < 4; i++){
                EEPROM.get((i+4)*sizeof(float),multiplier[i]);
                if (isnan(multiplier[i])){
                    multiplier[i] = 1;
                }
            }
            EEPROM.end();
        } else {
            for (int i = 0; i < 4; i++){
                offset[i] = 0;
            }
            for (int i = 0; i < 4; i++){
                multiplier[i] = 1;
            }
        }

        for (int i = 0; i < 4; i ++){
            Serial.println("channel " + String(i) + " offset value: " + String(offset[i]));
        }

        for (int i = 0; i < 4; i ++){
            Serial.println("channel " + String(i) + " multiplier value: " + String(multiplier[i]));
        }
    }



    float interpolate1000(double rawValue) {
        return ((rawValue - 0.5) * 250) + 12.5;
    }

    float interpolate5000(double rawValue) {
        return (rawValue * 1000 * 1.0042) + 5; //1.0042 from the voltage divider - 5647ohm and 5600ohm
    }

    float readPressurantPT1() {
        //Serial.print("Pressurant PT 1 reading: ");
        //Serial.println(max((float)1, interpolate5000(_upstreamPT1)));
        return max((float)1, multiplier[0] * (interpolate5000(_upstreamPT1) + offset[0]));
    }
    float readPressurantPT2() {
        // Serial.print("Pressurant PT 2 reading: ");
        // Serial.println(max((float)1, interpolate5000(_upstreamPT2)));
        return max((float)1, multiplier[2] * (interpolate5000(_upstreamPT2) + offset[2]));
    }

    float readTankPT1() {
        // Serial.print("Tank PT 1 reading: ");
        // Serial.println(max((float)1, interpolate1000(_downstreamPT1)));
        return max((float)1, multiplier[1] * (interpolate1000(_downstreamPT1) + offset[1]));
    }

    float readTankPT2() {
        // Serial.print("Tank PT 2 reading: ");
        // Serial.println(max((float)1, interpolate1000(_downstreamPT2)));
        return max((float)1, multiplier[3] * (interpolate1000(_downstreamPT2) + offset[3]));
    }

    //multiplier
    
    float readRawTankPT1() {
        // Serial.print("Raw Tank PT 1 reading: ");
        // Serial.println(multiplier[1] * (interpolate1000(_downstreamPT1) + offset[1]));
        return multiplier[1] * (interpolate1000(_downstreamPT1) + offset[1]);
    }
    float readRawTankPT2() {
        // Serial.print("Raw Tank PT 2 reading: ");
        // Serial.println(multiplier[3] * (interpolate1000(_downstreamPT2) + offset[3]));
        return multiplier[3] * (interpolate1000(_downstreamPT2) + offset[3]);
    }
    float readRawPressurantPT1() {
        // Serial.print("Raw Pressurant PT 1 reading: ");
        // Serial.println(multiplier[0] * (interpolate5000(_upstreamPT1) + offset[0]));
        return multiplier[0] * (interpolate5000(_upstreamPT1) + offset[0]);
    }
    float readRawPressurantPT2() {
        // Serial.print("Raw Pressurant PT 2 reading: ");
        // Serial.println(multiplier[2] * (interpolate5000(_upstreamPT2) + offset[2]));
        return multiplier[2] * (interpolate5000(_upstreamPT2) + offset[2]);
    }

    float readFilteredTankPT1() {
        if(millis() % 1000 == 0){
        Serial.print("Tank PT 1 reading: ");
        Serial.println(downstreamPT1Buff->getFiltered());
         }
        return (float) downstreamPT1Buff->getFiltered();
    }
    float readFilteredTankPT2() {
        if(millis() % 1000 == 0){
        Serial.print("Tank PT 2 reading: ");
        Serial.println(downstreamPT2Buff->getFiltered());
     }
        return (float) downstreamPT2Buff->getFiltered();
    }
    float readFilteredPressurantPT1() {
        if(millis() % 1000 == 0){
        Serial.print("Pressurant PT 1 reading: ");
        Serial.println(upstreamPT1Buff->getFiltered());
     }
        return (float) upstreamPT1Buff->getFiltered();
    }
    float readFilteredPressurantPT2() {
        if(millis() % 1000 == 0){
        Serial.print("Pressurant PT 2 reading: ");
        Serial.println(upstreamPT2Buff->getFiltered());
     }
        return (float) upstreamPT2Buff->getFiltered();
    }

    /**
     * @brief choose which ducer to read, implementing PT redundancy
     * @param ducer1 ducer 1 reading
     * @param ducer2 ducer 2 reading
     * @return *float* which ducer to use
    */
   float chooseDucerRead(float ducer1, float ducer2) {
        if ((ducer1 < -50) && (ducer2 < -50)) {
            return 10000; //easy way to abort
        }
       return max(ducer1, ducer2);
   }


}