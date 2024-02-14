#include "Radio.h"

namespace Radio {

    mode radioMode;
    int txInterval = 1000;

    uint8_t radioBuffer[MAX_RADIO_TRX_SIZE];
    uint8_t radioBufferSize = 0;

    volatile bool transmitting = false;
    long transmitStart = 0;

    #define PACKET_BUFFER_SIZE 20

    volatile SemaphoreHandle_t stackMutex = NULL;
    volatile uint8_t packetStack[PACKET_BUFFER_SIZE][MAX_RADIO_TRX_SIZE];
    volatile uint8_t packetStackLengths[PACKET_BUFFER_SIZE];
    volatile uint8_t packetStackSize = 0;
    uint8_t stackPacketTransmitBuffer[MAX_RADIO_TRX_SIZE]; //for the second core to copy data into

    volatile uint32_t cumPacketsPushed = 0;
    volatile uint32_t cumPacketsTransmitted = 0;



    volatile recvRadio_t recvRadio;

    char packetBuffer[sizeof(Comms::Packet)];

    Comms::Packet rssiPacket = {.id = FC_RSSI};

    bool enabled = false;
    
    void initRadio() {

        Si446x_init();
        Si446x_setTxPower(127);
        Si446x_setupCallback(SI446X_CBS_SENT, 1); 

        stackMutex = xSemaphoreCreateMutex();

        radioMode = TX;
        Serial.println("Starting in flight mode");
        enabled = true;
    }

     void processTransmitStack() {
        //this is only run from core 1!! (second core) 
        if (!xSemaphoreTake(stackMutex, 0xffffffUL)) { //~0.5s timeout
            Serial.println("C1: Failed to take mutex");
            return;
        }
        if(packetStackSize == 0){
            xSemaphoreGive(stackMutex);
            return;
        } 
        memcpy(stackPacketTransmitBuffer, (void*)packetStack[packetStackSize - 1], packetStackLengths[packetStackSize - 1]);
        uint8_t len = packetStackLengths[packetStackSize - 1];
        packetStackSize--;
        xSemaphoreGive(stackMutex);

        while (!Si446x_TX(stackPacketTransmitBuffer, len, 0, SI446X_STATE_RX));
        cumPacketsTransmitted++;
        Serial.printf("C1: Transmitted radio packet. Cum packets transmitted: %d\n", cumPacketsTransmitted);
    }

    void transmitRadioBuffer(bool swapFlag){
        if (!enabled) {
            return;
        }
        if(radioBufferSize == 0){
            return;
        }
        if(swapFlag){
            radioBuffer[radioBufferSize] = 255;
            radioBufferSize++;
        }

        bool success = false;

        if (!xSemaphoreTake(stackMutex, 0xffffffUL)) { //~0.5s timeout
            Serial.println("C0: Failed to take mutex");
            return;
        } 
        if(packetStackSize < PACKET_BUFFER_SIZE){
            memcpy((uint8_t*)packetStack[packetStackSize], radioBuffer, radioBufferSize);
            packetStackLengths[packetStackSize] = radioBufferSize;
            packetStackSize++;
            success = true;
        } else{
            memcpy((uint8_t*)packetStack[packetStackSize - 1], radioBuffer, radioBufferSize);
            packetStackLengths[packetStackSize - 1] = radioBufferSize;
        }
        xSemaphoreGive(stackMutex);
        if (success) {
            cumPacketsPushed++;
            Serial.printf("C0: Added packet to stack, size: %d. Cum packets pushed to stack: %d\n", radioBufferSize, cumPacketsPushed);
        } else {
            Serial.printf("C0: Stack full, overwrote top packet, size: %d\n", radioBufferSize);
        }
        radioBufferSize = 0;
    }

    void transmitRadioBuffer(){ transmitRadioBuffer(false);}

    void forwardPacket(Comms::Packet *packet){
        if (!enabled) {
            return;
        }
        // BlackBox::writePacket(packet);
        //Serial.println("forwarding packet");
        int packetLen = packet->len + 8;
        if(radioBufferSize + packetLen > MAX_RADIO_TRX_SIZE - 1){
            // Serial.printf("Attempting to transmit buffer w size  %d\n", radioBufferSize+packetLen);
            transmitRadioBuffer();
        }
        memcpy(radioBuffer + radioBufferSize, (uint8_t *) packet, packetLen);
        radioBufferSize += packetLen;
        
        //BlackBox::writePacket(packet);
    }



    void processRadio() {
        if (!enabled) {
            return;
        }
        Si446x_SERVICE();
    }
}