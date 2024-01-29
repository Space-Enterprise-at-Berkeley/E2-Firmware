#include <Arduino.h>

#include "Comms.h"
#include "Radio.h"
#include <Si446x.h>


void SI446X_CB_SENT(void)
{
    Radio::transmitting = false;
}

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{   
    if(length > MAX_RADIO_TRX_SIZE) length = MAX_RADIO_TRX_SIZE;

    Radio::recvRadio.ready = 1;
    Radio::recvRadio.rssi = rssi;
    Radio::recvRadio.length = length;

    Si446x_read((uint8_t*)Radio::recvRadio.buffer, length);
    Si446x_RX(0);
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	Si446x_RX(0);

	// Printing to serial inside an interrupt is bad!
	// If the serial buffer fills up the program will lock up!
	// Don't do this in your program, this only works here because we're not printing too much data
	Serial.print(F("Packet CRC failed (RSSI: "));
	Serial.print(rssi);
	Serial.println(F(")"));
}

void setup() 
{
  Serial.begin(921600);
  Serial.println("starting up");
  
  Radio::initRadio();
  
  Serial.println("hi");
  Serial.println("HII");
  Serial.println("starting in flight mode");

}

int delayS;

long sizePacketPeriod = 1e7;
long lastTime = micros();
long lastTimeMillis = millis();

void loop() {


  Comms::Packet tmp = {.id = 3};
  Comms::packetAddUint32(&tmp, 42);
  Comms::packetAddUint32(&tmp, 69);
  Comms::packetAddUint32(&tmp, 69);
  Comms::packetAddUint32(&tmp, 69);
  Comms::packetAddUint32(&tmp, 69);
  Comms::packetAddUint32(&tmp, 69);
  Comms::packetAddUint32(&tmp, 69);
  Comms::packetAddUint32(&tmp, 69);
  Comms::packetAddUint32(&tmp, 42);

  Comms::emitPacket(&tmp);
  Serial.println("GOING");

  Si446x_SERVICE();

}
