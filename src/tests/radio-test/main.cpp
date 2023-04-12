#include <Arduino.h>

#include <EspComms.h>
#include "Radio.h"
#include <Si446x.h>
int arr[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
int ctr = 0;

long sizePacketPeriod = 1e6;
long lastTime = micros();

Comms::Packet sizePacket = {.id = 153};

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
  Comms::init();
  Radio::initRadio();
  Serial.println("hi");
//   pinMode(33, OUTPUT);
  Serial.println("HII");
}
void loop() {
  if (micros() - lastTime > sizePacketPeriod) {
    sizePacket.len = 0;
    Comms::packetAddUint8(&sizePacket, 42);
    Radio::forwardPacket(&sizePacket);
    lastTime = micros();
  }
//   Comms::processWaitingPackets();

  //Comms::processWaitingPackets();
  //Radio::txCalib10(arr, 0);
  //delay(1000);
  // digitalWrite(33, LOW);
  // digitalWrite(33, HIGH);


  // Radio::txZeros(33); //tx's [0, 1, 2]
  // delayMicroseconds(2000000);  
  // Radio::txZeros(33); //tx's [0, 1, 2]
  // delayMicroseconds(5000000);
}