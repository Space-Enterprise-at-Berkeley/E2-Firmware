// #include <Arduino.h>
// #include <Wire.h>
// #include <MS5xxx.h>

// MS5xxx sensor(&Wire);

// void setup() {
//     Serial.begin(921600);
//     Serial.println("Initializing....");
//     Wire.setClock(400000);
//     Wire.setPins(1,2);
//     Wire.begin();
//     if(sensor.connect()>0) {
//       Serial.println("Error connecting...");
//       delay(500);
//       setup();
//   }
// }

// void test_crc() {
//   sensor.ReadProm();
//   sensor.Readout(); 
//   Serial.print("CRC=0x");
//   Serial.print(sensor.Calc_CRC4(), HEX);
//   Serial.print(" (should be 0x");
//   Serial.print(sensor.Read_CRC4(), HEX);
//   Serial.print(")\n");
//   Serial.print("Test Code CRC=0x");
//   Serial.print(sensor.CRCcodeTest(), HEX);
//   Serial.println(" (should be 0xB)");
// }

// void loop() {
//   sensor.ReadProm();
//   sensor.Readout();
//   Serial.print("Temperature [0.01 C]: ");
//   Serial.println(sensor.GetTemp());
//   Serial.print("Pressure [Pa]: ");
//   Serial.println(sensor.GetPres());
//   test_crc();
//   Serial.println("---");
//   delay(500);
// }



#include <Arduino.h>
#include <Wire.h> 
#include <BMP388_DEV.h>

BMP388_DEV bmp388;
    
float baroAltitude, baroPressure, baroTemperature;

uint32_t sampleAltPressTemp() {
    Serial.println("BAROMETER SAMPLING!!!!");
    bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
    Serial.println("Starting measuring");
    bmp388.getMeasurements(baroTemperature, baroPressure, baroAltitude);
    return 0;
}

void setup() { 
    Serial.begin(921600);
    Serial.println("Initializing....");
    Wire.setClock(400000);
    // Wire.setPins(1,2);
    Wire.begin(1, 2);

    bmp388.begin(0x76);
}

void loop() {
    sampleAltPressTemp();
    Serial.print("Barometer ALTITUDE: ");
    Serial.println(baroAltitude);
    Serial.print("Barometer PRESSURE: ");
    Serial.println(baroPressure);
    Serial.print("Barometer TEMPERATURE: ");
    Serial.println(baroTemperature);
    delay(150);
}