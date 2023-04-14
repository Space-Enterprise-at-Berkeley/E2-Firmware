#include <Arduino.h>

#include "Comms.h"
#include "Radio.h"
#include <Si446x.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS


SFE_UBLOX_GNSS myGNSS;
float latitude = 0;
float longitude = 0;
byte SIV = 0;
float altitude = 0;

// long lastTime1 = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.


int arr[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
int ctr = 0;
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
  #ifdef FLIGHT
  Serial.println("starting in flight mode");
  #endif
  #ifdef REPLAY
  Serial.println("replaying flight...");
  delay(1000);
  #endif

  Serial.println("starting gps setup for funsies");
  Wire.begin(1, 2);

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

}

int delayS;

long sizePacketPeriod = 1e7;
long lastTime = micros();
long lastTimeMillis = millis();

void loop() {
  // send blackbox size used
  static int cnt = 0;
  long t = micros();
  if (t - lastTime > sizePacketPeriod) {

    // if (t - lastTime > 1000*1000) { 
      latitude = myGNSS.getLatitude()/1e7;
      Serial.print(F("Lat: "));
      Serial.print(latitude);

      longitude = myGNSS.getLongitude()/1e7;
      Serial.print(F(" Long: "));
      Serial.print(longitude);
      Serial.print(F(" (degrees)"));

      altitude = myGNSS.getAltitude()/1e3;
      Serial.print(F(" Alt: "));
      Serial.print(altitude);
      Serial.print(F(" (m)"));

      SIV = myGNSS.getSIV();
      Serial.print(F(" SIV: "));
      Serial.println(SIV);
    // }

    // Serial.print("transmitting... ");
    // Serial.println(cnt++);
    // for (int i = 0; i < 60; i++) {
    // }

  

    lastTime = micros();
  }

    Comms::Packet tmp = {.id = 12};
    Comms::packetAddFloat(&tmp, latitude);
    Comms::packetAddFloat(&tmp, longitude);
    Comms::packetAddUint8(&tmp, SIV);
    Comms::packetAddFloat(&tmp, altitude);
    Comms::packetAddUint32(&tmp, 42);
    Comms::packetAddUint32(&tmp, 69);
    Comms::packetAddUint32(&tmp, 69);
    Comms::packetAddUint32(&tmp, 69);
    Comms::packetAddUint32(&tmp, 69);
    Comms::packetAddUint32(&tmp, 69);
    Comms::packetAddUint32(&tmp, 69);
    Comms::packetAddUint32(&tmp, 69);
    Comms::emitPacket(&tmp);

  Si446x_SERVICE();

  // if (millis() - lastTimeMillis > 1000) {

  //   lastTimeMillis = millis();

  //   latitude = myGNSS.getLatitude();
  //   Serial.print(F("Lat: "));
  //   Serial.print(latitude/(1e7));

  //   longitude = myGNSS.getLongitude();
  //   Serial.print(F(" Long: "));
  //   Serial.print(longitude/(1e7));
  //   Serial.print(F(" (degrees)"));

  //   altitude = myGNSS.getAltitude();
  //   Serial.print(F(" Alt: "));
  //   Serial.print(altitude);
  //   Serial.print(F(" (mm)"));

  //   SIV = myGNSS.getSIV();
  //   Serial.print(F(" SIV: "));
  //   Serial.println(SIV);
  // }

  // if (micros() - lastTime > 5e4)
  // {
  //   // lastTime1 = millis(); //Update the timer
    
  //   latitude = myGNSS.getLatitude();
  //   Serial.print(F("Lat: "));
  //   Serial.print(latitude/(1e7));

  //   longitude = myGNSS.getLongitude();
  //   Serial.print(F(" Long: "));
  //   Serial.print(longitude/(1e7));
  //   Serial.print(F(" (degrees)"));

  //   long altitude = myGNSS.getAltitude();
  //   Serial.print(F(" Alt: "));
  //   Serial.print(altitude);
  //   Serial.print(F(" (mm)"));

  //   byte SIV = myGNSS.getSIV();
  //   Serial.print(F(" SIV: "));
  //   Serial.print(SIV);

  //   Serial.println();
  // }
}
