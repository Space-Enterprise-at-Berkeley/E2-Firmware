#include "Ducers.h"
#include "EEPROM.h"


namespace Ducers {
    ADS8167 adc1;
    // SPIClass *spi2; 
    SoftSPI spi2Obj(41,42,45);
    SoftSPI* spi2 = &spi2Obj;
    

    uint32_t ptUpdatePeriod = 50 * 1000;
    Comms::Packet ptPacket = {.id = FC_PT_DATA};
    const uint8_t numPT = 2;
    float data[numPT];
    float offset[numPT];
    float multiplier[numPT];
    bool persistentCalibration = true;
    uint8_t channelCounter = 0;
    // uint8_t rtd0Channel = 1;
    // uint8_t rtd1Channel = 5;

    void handleFastReadPacket(Comms::Packet tmp, uint8_t ip) {
        if(tmp.data[0]) {
            ptUpdatePeriod = 1 * 1000;
        } else {
            ptUpdatePeriod = 100 * 1000;
        }
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

    float zeroChannel(uint8_t channel){
        offset[channel] = -data[channel] + offset[channel];
        Serial.println("zeroed channel " + String(channel) + " to " + String(offset[channel]));
        if (persistentCalibration){
            EEPROM.begin((numPT*2+2)*sizeof(float));
            EEPROM.put((channel+2)*sizeof(float),offset[channel]);
            EEPROM.end();
        }
        return offset[channel];
    }

    float calChannel(uint8_t channel, float value){
        multiplier[channel] *= (value) / data[channel];
        Serial.println("calibrated channel multiplier" + String(channel) + " to " + String(multiplier[channel]));
        if (persistentCalibration){
            EEPROM.begin((numPT*2+2)*sizeof(float));
            EEPROM.put((channel+numPT+2)*sizeof(float),multiplier[channel]);
            EEPROM.end();
        }
        return multiplier[channel];
    }

    //sets offset (y-int)
    void onZeroCommand(Comms::Packet packet, uint8_t ip){
        uint8_t channel = Comms::packetGetUint8(&packet, 0);
        zeroChannel(channel);
    }

    //uses current value and given value to add multiplier (slope) to match two points
    void onCalCommand(Comms::Packet packet, uint8_t ip){
        uint8_t channel = Comms::packetGetUint8(&packet, 0);
        float value = Comms::packetGetFloat(&packet, 1);

        calChannel(channel, value);
    }

    void sendCal(Comms::Packet packet, uint8_t ip){
        Comms::Packet response = {.id = SEND_CAL, .len = 0};
        for (int i = 0; i < numPT; i++){
            Comms::packetAddFloat(&response, offset[i]);
            Comms::packetAddFloat(&response, multiplier[i]);
            Serial.println("Channel " + String(i) + ": offset " + String(offset[i]) + ", multiplier " + String(multiplier[i]));
        }
        Comms::emitPacketToGS(&response);
        WiFiComms::emitPacketToGS(&response);
        //Radio::forwardPacket(&response);
    }

    void resetCal(Comms::Packet packet, uint8_t ip){
        uint8_t channel = Comms::packetGetUint8(&packet, 0);
        offset[channel] = 0;
        multiplier[channel] = 1;
        if (persistentCalibration){
            EEPROM.begin((numPT*2+2)*sizeof(float));
            EEPROM.put((channel+2)*sizeof(float),offset[channel]);
            EEPROM.put((channel+numPT+2)*sizeof(float),multiplier[channel]);
            EEPROM.end();
        }
    }

    void init() {
        // Comms::registerCallback(140, handleFastReadPacket);
        // spi2 = new SPIClass(HSPI);
        spi2->begin();
        spi2->setClockDivider(SPI_CLOCK_DIV16);
        spi2->setDataMode(SPI_MODE0);

        // spi2->begin(45, 42, 41, 40);
        adc1.init(spi2, 40, 46);
        adc1.setAllInputsSeparate();
        adc1.enableOTFMode();

        Comms::registerCallback(FC_PT_ZERO, onZeroCommand);
        Comms::registerCallback(FC_PT_CAL, onCalCommand);
        Comms::registerCallback(FC_PT_SEND_CAL, sendCal);
        Comms::registerCallback(FC_PT_RESET_CAL, resetCal);

        //load offset from flash or set to 0
        if (persistentCalibration){
            EEPROM.begin((numPT*2+2)*sizeof(float));
            for (int i = 0; i < numPT; i++){
                EEPROM.get((i+2)*sizeof(float),offset[i]);
                if (isnan(offset[i])){
                    offset[i] = 0;
                }
            }
            for (int i = 0; i < numPT; i++){
                EEPROM.get((i+2+numPT)*sizeof(float),multiplier[i]);
                if (isnan(multiplier[i])){
                    multiplier[i] = 1;
                }
            }
            EEPROM.end();
        } else {
            for (int i = 0; i < numPT; i++){
                offset[i] = 0;
            }
            for (int i = 0; i < numPT; i++){
                multiplier[i] = 1;
            }
        }

    }


    float samplePT(uint8_t channel) {
        adc1.setChannel(channel);
        data[channel] = multiplier[channel] * (interpolate1000(adc1.readChannelOTF(channel)) + offset[channel]);
        return data[channel];
    }

    float noSamplePT(uint8_t channel){
        return data[channel];
    }

    uint32_t task_ptSample() {


        // read from all 8 PTs in sequence
        if (channelCounter == 0){
             Comms::emitPacketToGS(&ptPacket);
             //WiFiComms::emitPacketToGS(&ptPacket);
             Radio::forwardPacket(&ptPacket);
             ptPacket.len = 0;
        }

        uint16_t raw =  adc1.readData(channelCounter);
        if (channelCounter == 0) {
            Serial.println(raw);

        }
        data[channelCounter] = multiplier[channelCounter] * (interpolate1000(raw) + offset[channelCounter]);
        // data[channelCounter] = raw;
        Comms::packetAddFloat(&ptPacket, data[channelCounter]);
        
        channelCounter = (channelCounter + 1) % numPT;

        return ptUpdatePeriod/numPT;
    }

    void print_ptSample(){
        for (int i = 0; i < numPT; i ++){
            Serial.print("  PT"+String(i)+": " + String(data[i]));
        }
        Serial.println();
    }


};