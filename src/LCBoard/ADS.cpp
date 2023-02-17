#include "ADS.h"




namespace ADS {
    Comms::Packet ADCPacket = {.id = 2};
    int clockPins[] = {36,37,26,35};
    int dataPins[] = {18,21,33,34};
    const int ADCsize = sizeof(dataPins)/sizeof(int);
    ADS1231 adcs[ADCsize];
    long data[sizeof(ADCsize)];
    uint32_t sampleRate = 12500; //80Hz


    void init(){
        for(int i = 0; i < ADCsize; i++) {
            adcs[i].init(clockPins[i],dataPins[i]);
        }
        
    }

    void refreshReadings(){
        for(int i = 0 ; i < ADCsize; i++){
            int ErrorValue = adcs[i].getValue(data[i]);
            if(ErrorValue != 1){//if we fail to read
                // Serial.println("failed to fetch loadcell data for " + String(i) + "th loadcell. error number: " + String(ErrorValue));
            }else{
                // Serial.println("succesfully read " + String(i) + "th loadcell " + String(data[i]));
            }; //write the new value into data[i]
            
        }
    }

    uint32_t printReadings(){
        // Serial.println("readings: " + String(data[3]));
        Serial.println("readings: " + String(data[0]) + " " + String(data[1]) + " " + String(data[2]) + " " + String(data[3]));
        return 125000; //8Hz
    }

    uint32_t task_sampleLC(){
        refreshReadings();

        ADCPacket.len = 0;
        for(int i = 0 ; i < ADCsize; i ++){
            Comms::packetAddFloat(&ADCPacket, data[i]); //write data[i] into the packet
        }
        Comms::emitPacket(&ADCPacket); //commented out for tesing. shoud comment back in for comms

        return sampleRate; //80Hz

    }

    long unrefreshedSample(int i){
        return data[i];
    }

}
#include "ADS.h"




namespace ADS {
    Comms::Packet ADCPacket = {.id = 4};
    int clockPins[] = {36,37,26,35};
    int dataPins[] = {18,21,33,34};
    const int ADCsize = sizeof(dataPins)/sizeof(int);
    ADS1231 adcs[ADCsize];
    long data[sizeof(ADCsize)];
    uint32_t sampleRate = 12500; //80Hz


    void init(){
        for(int i = 0; i < ADCsize; i++) {
            adcs[i].init(clockPins[i],dataPins[i]);
        }
        
    }

    void refreshReadings(){
        for(int i = 0 ; i < ADCsize; i++){
            int ErrorValue = adcs[i].getValue(data[i]);
            if(ErrorValue != 1){//if we fail to read
                // Serial.println("failed to fetch loadcell data for " + String(i) + "th loadcell. error number: " + String(ErrorValue));
            }else{
                // Serial.println("succesfully read " + String(i) + "th loadcell " + String(data[i]));
            }; //write the new value into data[i]
            
        }
    }

    uint32_t printReadings(){
        // Serial.println("readings: " + String(data[3]));
        Serial.println("readings: " + String(data[0]) + " " + String(data[1]) + " " + String(data[2]) + " " + String(data[3]));
        return 125000; //8Hz
    }

    uint32_t task_sampleLC(){
        refreshReadings();

        ADCPacket.len = 0;
        for(int i = 0 ; i < ADCsize; i ++){
            Comms::packetAddFloat(&ADCPacket, data[i]); //write data[i] into the packet
        }
        Comms::emitPacket(&ADCPacket); //commented out for tesing. shoud comment back in for comms

        return sampleRate; //80Hz

    }

}