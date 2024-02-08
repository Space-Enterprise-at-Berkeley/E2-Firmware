#include "Radio.h"

namespace Radio {

    mode radioMode;
    int txInterval = 1000;

    uint8_t radioBuffer[MAX_RADIO_TRX_SIZE];
    uint8_t radioBufferSize = 0;

    volatile bool transmitting = false;
    long transmitStart = 0;

    volatile recvRadio_t recvRadio;

    char packetBuffer[sizeof(Comms::Packet)];

    Comms::Packet rssiPacket = {.id = FC_RSSI};

    bool enabled = false;
    
    void initRadio() {

        Si446x_init();
        Si446x_setTxPower(127);
        Si446x_setupCallback(SI446X_CBS_SENT, 1); 

        radioMode = TX;
        Serial.println("Starting in flight mode");
        enabled = true;
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
        bool success = Si446x_TX(radioBuffer, radioBufferSize, 0, SI446X_STATE_RX);
        transmitting = true;
        //digitalWrite(RADIO_LED, LOW);
        transmitStart = millis();
        Serial.println("Transmitting Radio Packet");
        if(!success){
            Serial.println("Error Transmitting Radio Packet");
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
            transmitRadioBuffer();
        }
        memcpy(radioBuffer + radioBufferSize, (uint8_t *) packet, packetLen);
        radioBufferSize += packetLen;
        
        BlackBox::writePacket(packet);
    }

    bool processWaitingRadioPacket() {
        if (!enabled) {
            return false;
        }
        if(recvRadio.ready == 1){
            Serial.print("Received radio packet of size ");
            Serial.println(recvRadio.length);

            int16_t lastRssi = recvRadio.rssi;
            Serial.print("RSSI:" );
            Serial.println(lastRssi);

            memcpy(radioBuffer, (uint8_t *)recvRadio.buffer, recvRadio.length);

            recvRadio.ready == 0;

            int idx = 0;
            while(idx<recvRadio.length){
                int packetID = radioBuffer[idx];
                if(packetID == 255){
                    radioBufferSize = 0;
                    return true;
                }

                int packetLen = radioBuffer[idx+1];

                memcpy(packetBuffer, (uint8_t *)radioBuffer + idx, packetLen+8);

                idx += packetLen + 8;

                Comms::Packet *packet = (Comms::Packet *) &packetBuffer;
                
                Comms::emitPacketToGS(packet);
                BlackBox::writePacket(packet);
            }
            float rssi = (float) recvRadio.rssi;
            rssiPacket.len = 0;
            Comms::packetAddFloat(&rssiPacket, rssi);
            Comms::emitPacketToGS(&rssiPacket);
            WiFiComms::emitPacketToGS(&rssiPacket);
            BlackBox::writePacket(&rssiPacket);

            recvRadio.ready = 0;
        }
        return false;
    }

    void processRadio() {
        if (!enabled) {
            return;
        }
        Si446x_SERVICE();
    }
}