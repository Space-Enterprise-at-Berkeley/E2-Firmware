#include "ADS.h"




namespace ADS {
    Comms::Packet ADCPacket = {.id = 4};
    int clockPins[] = {36,37,26,35};
    int dataPins[] = {18,21,33,34};
    const int ADCsize = sizeof(dataPins);
    ADS1231 adcs[ADCsize];
    long data[sizeof(ADCsize)];


    void init(){
        for(int i = 0; i < size; i++) {
            adcs[i] = ADS1231(clockPins[i],dataPins[i]);
        }
        
    }

    void refreshReadings(){
        for(int i = 0 ; i < size; i ++){
            adcs[i].getValue(data[i]); //write the new value into data[i]
        }
    }

    uint32_t sampleLC(uint16_t i){
        refreshReadings();

        ADCPacket.len = 0;
        for(int i = 0 ; i < size; i ++){
            Comms::packetAddFloat(&ADCPacket, data[i]); //write data[i] into the packet
        }
        Comms::emitPacket(&ADCPacket);

        return 12500; //80Hz

    }

}