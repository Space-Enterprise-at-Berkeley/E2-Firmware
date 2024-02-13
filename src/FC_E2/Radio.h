#pragma once

#include <Arduino.h>
#include <Si446x.h>
#include "WiFiCommsLite.h"
#include "Radio.h"
#include "BlackBox.h"

namespace Radio {   
    enum mode {TX, RX, IDLE};
    extern mode radioMode;

    #define MAX_RADIO_TRX_SIZE 128
    #define PACKET_BUFFER_SIZE 20

    typedef struct{
	uint8_t ready;
	int16_t rssi;
	uint8_t length;
	uint8_t buffer[MAX_RADIO_TRX_SIZE];
    } recvRadio_t;




    extern volatile recvRadio_t recvRadio;

    extern int txInterval;
    extern volatile bool transmitting;

    void initRadio();

    void transmitRadioBuffer(bool swapFlag);
    void transmitRadioBuffer();
    void transmitTestPattern() ;

    void forwardPacket(Comms::Packet *packet);
    bool processWaitingRadioPacket();

    void processRadioBuffer();
    void processRadio();
}