#include "Ducers.h"

namespace Ducers {

    //set on call of HAL::readAllDucers(). 
    float _upstreamPT1;
    float _downstreamPT1;
    float _upstreamPT2;
    float _downstreamPT2;

    void setDownstreamPT1(float downstreamPT1) {_downstreamPT1 = downstreamPT1;}
    void setDownstreamPT2(float downstreamPT2) {_downstreamPT2 = downstreamPT2;}
    void setUpstreamPT1(float upstreamPT1) {_upstreamPT1 = upstreamPT1;}
    void setUpstreamPT2(float upstreamPT2) {_upstreamPT2 = upstreamPT2;}


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
    float readTankPT2() {
        return max((float)1, interpolate1000(_downstreamPT2));
    }

}