

#include "Barometer.h"

float GROUND_PRESSURE_HPA = 1013;
float baro_altitude = 0;


float baro_temp = 0;


float baro_pressure_2 = 0;
float baro_temp_2 = 0;

Adafruit_BMP3XX bmp;


MS5xxx ms5607(&Wire);


namespace Barometer 
{ 

  void set_barometer_ground_altitude() {
    float output = 0;
    for (int i=0; i < 100; i++) {
        output += (bmp.readPressure() / 100.0);
    }
    output = output / 100;
    GROUND_PRESSURE_HPA = output;
  }


  float get_barometer_altitude() {
    float output;
    output = bmp.readAltitude(GROUND_PRESSURE_HPA);
    return output;
  }


  float get_barometer_temperature() {
    float output;
    output = bmp.temperature;
    return output;
  }

  float get_barometer_pressure() {
    float output;
    output = bmp.pressure / 100.0;
    return output;
  }

//   float get_baro2_temp() { 
//     return ms5607.GetTemp();
//   }

  void read_baro2() { 
    ms5607.ReadProm();
    ms5607.Readout();
    baro_pressure_2 = ms5607.GetPres();
    baro_temp_2 = ms5607.GetTemp();
  }

  unsigned long lastExecutedMillis = 0; // create a variable to save the last executed time

  uint32_t updateRate = 100 * 1000;

 
  void init(){


    if (!bmp.begin_I2C(0x77)) {
      Serial.println("Could not find a valid BMP3 sensor, check wiring!");
      while (1);
    }


    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_200_HZ);
    bmp.performReading();
    
    set_barometer_ground_altitude();

    if(ms5607.connect()>0) {
      Serial.println("Error connecting...");
    }
  }

  

  // Packet definitions
  Comms::Packet baroPacket = {.id = 5};
  

  uint32_t updateFlight() { 
    baro_altitude = get_barometer_altitude();
    
    baro_temp = get_barometer_temperature();

    // read_baro2();

    Serial.print("baroAltitude:"); Serial.print(baro_altitude); Serial.print(" ");


    // for (int i = 0; i < 10; i++) { 

      baroPacket.len = 0;
      Comms::packetAddFloat(&baroPacket, baro_altitude);
      Comms::packetAddFloat(&baroPacket, baro_temp);
      Comms::packetAddFloat(&baroPacket, baro_pressure_2);
      Comms::packetAddFloat(&baroPacket, baro_temp_2);

      // emit the packets
      Comms::emitPacketToGS(&baroPacket);
      Radio::forwardPacket(&baroPacket);

    // }

    return updateRate;
  }

}