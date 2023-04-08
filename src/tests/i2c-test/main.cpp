
#include <Arduino.h>
#include <Wire.h>

// Replace these values with the correct ones for your I2C device
const byte DEVICE_ADDRESS = 0x76; // I2C device address
const byte REGISTER_ADDRESS = 0x04; // Register address to read from

void setup() {
  Serial.begin(921600);
  Serial.println("Initializing....");
  Wire.setClock(400000);
  Wire.setPins(1,2);
  Wire.begin();
}

void loop() {
  byte data; // Variable to store the received data

  // Request data from the I2C device
  Wire.beginTransmission(DEVICE_ADDRESS); // Start I2C communication with the device
  Wire.write(REGISTER_ADDRESS); // Send register address to read from
  Wire.endTransmission(false); // End transmission without releasing the I2C bus (repeated start)

  Wire.requestFrom(DEVICE_ADDRESS, (byte)1); // Request 1 byte of data from the device

  if (Wire.available()) { // Check if data is available
    data = Wire.read(); // Read the received byte
    Serial.print("Register Value: ");
    Serial.println(data, HEX); // Print the received data in hexadecimal format
  } else {
    Serial.println("No data available.");
  }

  delay(1000); // Wait 1 second before repeating the process
}