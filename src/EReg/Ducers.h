#include "Arduino.h"
#include "HAL.h"
#include "Util.h"

namespace Ducers {
    void readAllPTs();

    void setDownstreamPT1(float downstreamPT1);
    void setDownstreamPT2(float downstreamPT2);
    void setUpstreamPT1(float upstreamPT1);
    void setUpstreamPT2(float upstreamPT2);

    float readPressurantPT1();
    float readTankPT1();
    float readPressurantPT2();
    float readTankPT1();
    float readRawTankPT1();
    float readRawTankPT2();
    float readRawPressurantPT1();
    float readRawPressurantPT2();
    
}