#include "ReadPower.h"

//reads power stats from INA233 and sends to ground station
namespace Power
{
    INA233 ina233(INA233_ADDRESS_41, Wire);
    float rShunt = 0.004;
    float iMax = 5.0;
    float sendRate = 500 * 1000; // 0.5 second
    
    Comms::Packet p = {.id = 1};

    void init()
    {
        // let'sa gooo!
        // Serial.begin(921600); Comms takes care of this
        DEBUG("Initializing INA...");

        //Wire.setClock(400000); 
        //Wire.setPins(1,2); These ain't my problem.
        //Wire.begin();

        ina233.init(rShunt,iMax);
    }

    uint32_t task_readSendPower()
    {
        // read the ina
        float busVoltage = ina233.readBusVoltage();
        float shuntCurrent = ina233.readCurrent();
        //float shuntVoltage = ina233.readShuntVoltage(); don't need this
        float power = ina233.readPower();
        //float avgPower = ina233.readAvgPower(); eh maybe?

        //make Packet
        p.len = 0;
        Comms::packetAddFloat(&p, busVoltage);
        Comms::packetAddFloat(&p, shuntCurrent);
        Comms::packetAddFloat(&p, power);

        // emit the packet
        Comms::emitPacketToGS(&p);

        return sendRate; // 1 second
    }

    void print()
    {
        // read the ina
        float busVoltage = ina233.readBusVoltage();
        float shuntCurrent = ina233.readCurrent();
        //float shuntVoltage = ina233.readShuntVoltage(); don't need this
        float power = ina233.readPower();
        //float avgPower = ina233.readAvgPower(); eh maybe?

        // print the ina
        Serial.print("Bus Voltage: ");
        Serial.println(busVoltage);
        Serial.print("Shunt Current: ");
        Serial.println(shuntCurrent);
        Serial.print("Power: ");
        Serial.println(power);
    }
}