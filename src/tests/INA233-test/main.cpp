/*
#include <Arduino.h>
#include <Wire.h>

#include <INA233.h>

INA233 ina;
float rShunt = 0.004;

void setup(){
    // cs, ready, alert
    Serial.begin(921600);
    Serial.println("Initializing....");

    ina.init(&Wire,INA233_ADDRESS_41,rShunt,5.0);
    Wire.setTimeout(1000);

    while(1) {
        Wire.beginTransmission(INA233_ADDRESS_41);
        Wire.write(READ_VIN);
        Serial.println(Wire.endTransmission());
        Serial.println("Bus Voltage:");
        //ina.writeRegister(READ_VIN);
        Wire.beginTransmission(INA233_ADDRESS_41);
        Serial.println("trans start");
        Serial.println(Wire.requestFrom(INA233_ADDRESS_41, 2));
        Serial.println("requested");
        Serial.flush();
        Serial.println(Wire.read());
        Serial.println(Wire.read());
        Serial.println(Wire.endTransmission());
        Serial.println(ina.readRegister16(READ_VIN));
        Serial.println("Shunt Current:");
        Serial.println(ina.readShuntCurrent_raw());
        delay(200);
    }

    return;
}

void loop(){

}
*/

/////////////////////////////////////////////////////////////////////
//
//  infinityPV_INA233_Read_Current_Power.ino
//
//  This is an example for using the LiBAT5V2A Energy Management shield    
//  from infinityPV with Arduino.                                    
//  The board contains 2 INA233 Analog to I2C converters             
//  to monitor energy input and output in the application:                      
//    IC2. Energy input to the battery 
//    IC1. Energy output to the application (5V output)
// 
//  The INA233 is able to monitor voltage, current, power and 
//  accumulated power (energy) at the measurement point.
//  
//  Here we show how to measure instantaneous power, current 
//  and voltage using the functions implemented in the library 
//  infintyPV_INA233.h
//
//  In the setup area we use the setCalibration() function
//  for each ones of the INA233 on board. 
//  This is needed in all the application which need to 
//  measure current or/and power. Check the example 
//  infinityPV_INA233_Setting_Calibration.ino for info. 

//  We have implemented:
//  - getCurrent_mA(), which returns the value (float) of measured current
//  flowing through the shunt resistor in mA. 
//  - getPower_mW(), which returns the value (float) of measured power
//  flowing through the shunt resistor in mW.
//
//  Also is you want to check the raw value (uint16_t) from internal registers:
//  -getCurrent_raw()
//  -getPower_raw()
//
// (*) The example uses the infinityPV_INA233 library, which licensed 
// under BSD license. 
//
//////////////////////////////////////////////////////////////////////


#include <Wire.h>
#include <infinityPV_INA233.h>

//I2C Adresses for the 2 INA233 on board 
INA233 IC1(0x41);  // IC1 address 0x45 1000101 

void setup() {
  //values to be introduced by the user 
  float R_shunt_IC1=0.004;   //call RS2 on the board 
  float I_max_IC1=2;

  //variables to catch the outputs from set_Calibration()
  uint16_t CAL=0;
  int16_t m_c=0;
  int16_t m_p=0;
  int8_t R_c=0;
  int8_t R_p=0;
  uint8_t Set_ERROR=0;
  float Current_LSB=0;
  float Power_LSB=0;
  //variable to check the loaded calibration
  uint16_t Read_CAL=0;
  
  Serial.begin(921600);
  Serial.println("Hej!");
  Serial.println("Measuring instantaneous values at IC1 and IC2 INA233 at LiBAT5V2A Energy Management Shield:");
  IC1.begin();

  // Call calibration function for each INA233 (Check infinityPV_INA233_Setting_Calibration.ino 
  // if you want to check the details about calibration.
  CAL=IC1.setCalibration(R_shunt_IC1,I_max_IC1,&Current_LSB,&Power_LSB,&m_c,&R_c,&m_p,&R_p,&Set_ERROR);

}

void loop() {
  int16_t raw_vbus_IC1=0;
  int16_t raw_vshunt_IC1=0;
  int16_t raw_current_IC1=0;
  int16_t raw_power_IC1=0;

  float vbus_IC1=0;
  float vshunt_IC1=0;
  float current_IC1=0;
  float power_IC1=0;

  //Reading raw values, if you do not need them, you can comment this section
  raw_vbus_IC1=IC1.getBusVoltage_raw();
  raw_vshunt_IC1=IC1.getShuntVoltage_raw();
  raw_current_IC1=IC1.getCurrent_raw();
  raw_power_IC1=IC1.getPower_raw();
  Serial.print(" Raw Bus voltage IC1:   "); Serial.print(raw_vbus_IC1,HEX);Serial.println("h");
  Serial.print(" Raw Shunt voltage IC1:   "); Serial.print(raw_vshunt_IC1,HEX);Serial.println("h");
  Serial.print(" Raw current IC1:   "); Serial.print(raw_current_IC1,HEX);Serial.println("h");
  Serial.print(" Raw power IC1:   "); Serial.print(raw_power_IC1,HEX);Serial.println("h");
  
  //Reading values in standard units
  vbus_IC1=IC1.getBusVoltage_V();
  vshunt_IC1=IC1.getShuntVoltage_mV();
  current_IC1=IC1.getCurrent_mA();
  power_IC1=IC1.getPower_mW();
  Serial.print(" Bus voltage IC1:   "); Serial.print(vbus_IC1);Serial.println(" V");
  Serial.print(" Shunt voltage IC1:   "); Serial.print(vshunt_IC1);Serial.println(" mV");
  Serial.print(" Current IC1:   "); Serial.print(current_IC1);Serial.println(" mA");
  Serial.print(" Power IC1:   "); Serial.print(power_IC1);Serial.println(" mW");

  delay(2000);
}
