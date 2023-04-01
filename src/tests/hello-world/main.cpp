// #include <Arduino.h>
// // #include <USBSerial.h>
// #include <Adafruit_TinyUSB.h>

// // USBSerial usbSerial;

// Adafruit_USBD_CDC usb_cdc;

// void setup() {
//     usb_cdc.begin(115200);
//     while (!usb_cdc) { 
//         delay(10);
//     }
// }

// void loop() {
//     if (usb_cdc.connected()) { 
//         usb_cdc.println("test");
//         delay(1000);
//     }
// }

#include <Arduino.h>

void setup() {
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  Serial.println("test");
  digitalWrite(34, HIGH);
  digitalWrite(35, LOW);
  delay(1000);
  digitalWrite(34, LOW);
  digitalWrite(35, HIGH);
  delay(1000);
}