

#include "GPS.h"

#define STATE_SIZE 3
#define OBS_SIZE 3
#define DELAY_TIME 1

float gps_altitude = 0;

float longitude = 0;
float latitude = 0;

SFE_UBLOX_GNSS neom9n;


namespace GPS 
{ 

  float get_gps_altitude() {
    // default is in mm
    return neom9n.getAltitude() / 1000;
  }

  float get_latitude() { 
    return neom9n.getLatitude() / 1e7;
  }

  float get_longitude() { 
    return neom9n.getLongitude() / 1e7;
  }

  uint8_t get_gps_sats() { 
    return (uint8_t)(neom9n.getSIV());
  }

  
  unsigned long lastExecutedMillis = 0; // create a variable to save the last executed time

  uint32_t updateRate = 100 * 1000;

  void init(){

    if (neom9n.begin() == false) //Connect to the u-blox module using Wire port
    {
      Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
      while (1);
    }

    neom9n.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    neom9n.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
    neom9n.setNavigationFrequency(40);
    neom9n.setAutoPVT(true);

  }
  

  // Packet definitions
  Comms::Packet gpsPacket = {.id = 4};

  uint32_t updateFlight() { 
    gps_altitude = get_gps_altitude();

    longitude = get_longitude();
    latitude = get_latitude();
    
    Serial.print("GPSAltitude:"); Serial.print(gps_altitude); Serial.print("  ");
    // for (int i = 0; i < 10; i++) { 

      gpsPacket.len = 0;
      Comms::packetAddFloat(&gpsPacket, gps_altitude);
      Comms::packetAddFloat(&gpsPacket, latitude);
      Comms::packetAddFloat(&gpsPacket, longitude);
      Comms::packetAddUint8(&gpsPacket, get_gps_sats());

      // emit the packets
      Comms::emitPacketToGS(&gpsPacket);
      Radio::forwardPacket(&gpsPacket);

    // }
    return updateRate;
  }

}