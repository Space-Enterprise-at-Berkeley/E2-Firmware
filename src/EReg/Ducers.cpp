#include "Ducers.h"

namespace Ducers {

    //set on call of HAL::readAllDucers(). 
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

    void initPTs() {
            upstreamPT1Buff = new Buffer(Config::PTFilterBufferSize);
            downstreamPT1Buff = new Buffer(Config::PTFilterBufferSize);
            upstreamPT2Buff = new Buffer(Config::PTFilterBufferSize);
            downstreamPT2Buff = new Buffer(Config::PTFilterBufferSize);
            upstreamPT1Buff->clear();
            downstreamPT1Buff->clear();
            upstreamPT2Buff->clear();
            downstreamPT2Buff->clear();
    }


    float interpolate1000(double rawValue) {
        return ((rawValue - 0.5) * 250) + 12.5;
    }

    float interpolate5000(double rawValue) {
        return (rawValue * 1000 * 1.0042) + 5; //1.0042 from the voltage divider - 5647ohm and 5600ohm
    }

    float readPressurantPT1() {
        return max((float)1, interpolate5000(_upstreamPT1));
    }
    float readPressurantPT2() {
        return max((float)1, interpolate5000(_upstreamPT2));
    }

    float readTankPT1() {
        return max((float)1, interpolate1000(_downstreamPT1));
    }

    float readTankPT2() {
        return max((float)1, interpolate1000(_downstreamPT2));
    }

    float readRawTankPT1() {
        return interpolate1000(_downstreamPT1);
    }
    float readRawTankPT2() {
        return interpolate1000(_downstreamPT2);
    }
    float readRawPressurantPT1() {
        return interpolate5000(_upstreamPT1);
    }
    float readRawPressurantPT2() {
        return interpolate5000(_upstreamPT2);
    }

    float readFilteredTankPT1() {
        return (float) downstreamPT1Buff->getFiltered();
    }
    float readFilteredTankPT2() {
        return (float) downstreamPT2Buff->getFiltered();
    }
    float readFilteredPressurantPT1() {
        return (float) upstreamPT1Buff->getFiltered();
    }
    float readFilteredPressurantPT2() {
        return (float) upstreamPT2Buff->getFiltered();
    }

    /**
     * @brief choose which ducer to read, implementing PT redundancy
     * @param ducer1 ducer 1 reading
     * @param ducer2 ducer 2 reading
     * @return *float* which ducer to use
    */
   float chooseDucerRead(float ducer1, float ducer2) {
       return max(ducer1, ducer2);
   }


}