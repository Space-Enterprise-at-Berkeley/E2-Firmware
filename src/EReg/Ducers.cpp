#include "Ducers.h"

namespace Ducers {

    int16_t upstreamPT;
    int16_t downstreamPT;

    void updateUpstreamPT() {
        upstreamPT = HAL::readPTVoltage(2);
    }

    void updateDownstreamPT() {
        downstreamPT = HAL::readPTVoltage(0);
    }

    float interpolate1000(double rawValue) {
        return rawValue * 250.38 - 123.87;
    }

    float interpolate5000(double rawValue) {
        return rawValue * 1310.15 + 26.65;
    }

    float readPressurantPT() {
        updateUpstreamPT();
        return max((float)1, interpolate5000(upstreamPT));
    }

    float readTankPT() {
        updateDownstreamPT();
        return max((float)1, interpolate1000(downstreamPT));
    }

    float readDownstreamPT() {
        return readTankPT();
    }

    float readUpstreamPT() {
        return readPressurantPT();
    }




}