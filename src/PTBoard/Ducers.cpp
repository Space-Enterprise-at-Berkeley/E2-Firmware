#include "Ducers.h"

namespace Ducers {
    ADS8167 adc1;
    

    uint32_t ptUpdatePeriod = 20 * 1000;
    Comms::Packet ptPacket = {.id = 10};

    // float pressurantPTValue = 0.0;
    // float loxTankPTValue = 0.0;
    // float fuelTankPTValue = 0.0;
    // float loxInjectorPTValue = 0.0;
    // float fuelInjectorPTValue = 0.0;
    // float loxDomePTValue = 0.0;
    // float fuelDomePTValue = 0.0;

    void handleFastReadPacket(Comms::Packet tmp, uint8_t ip) {
        if(tmp.data[0]) {
            ptUpdatePeriod = 1 * 1000;
        } else {
            ptUpdatePeriod = 100 * 1000;
        }
    }

    void initDucers() {
        // Comms::registerCallback(140, handleFastReadPacket);
        adc1.init(&SPI, 37, 25);
        adc1.setAllInputsSeparate();
        adc1.enableOTFMode();
    }

    float interpolate1000(uint16_t rawValue) {
        // TODO multiply rawValue by 2
        float tmp = (float) (rawValue - 6406);
        return tmp / 51.7;
    }

    float interpolate5000(uint16_t rawValue) {
        float tmp = (float) rawValue;
        return tmp / 12.97;
    }

    uint32_t ptSample() {
        // read from all 6 PTs in sequence
        
        adc1.readChannelOTF(1); // switch mux back to channel 1
        float ch0 = interpolate1000(adc1.readChannelOTF(0));
        float ch1 = interpolate1000(adc1.readChannelOTF(1));
        float ch2 = interpolate1000(adc1.readChannelOTF(2));
        float ch3 = interpolate1000(adc1.readChannelOTF(3));
        float ch4 = interpolate1000(adc1.readChannelOTF(4));
        float ch5 = interpolate1000(adc1.readChannelOTF(5)); 
        float ch6 = interpolate1000(adc1.readChannelOTF(6));
        float ch7 = interpolate1000(adc1.readChannelOTF(7));

        DEBUG("Read all PTs\n");
        DEBUG_FLUSH();

        // emit a packet with data
        ptPacket.len = 0;
        Comms::packetAddFloat(&ptPacket, ch0);
        Comms::packetAddFloat(&ptPacket, ch1);
        Comms::packetAddFloat(&ptPacket, ch2);
        Comms::packetAddFloat(&ptPacket, ch3);
        Comms::packetAddFloat(&ptPacket, ch4);
        Comms::packetAddFloat(&ptPacket, ch5);
        Comms::packetAddFloat(&ptPacket, ch6);
        Comms::packetAddFloat(&ptPacket, ch7);

        Comms::emitPacket(&ptPacket);
        // Comms::emitPacket(&ptPacket, &RADIO_SERIAL, "\r\n\n", 3);
        // return the next execution time
        DEBUG("PT Packet Sent\n");
        DEBUG_FLUSH();

        return ptUpdatePeriod;
    }

};
