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
#include <tusb.h>

void setup() {
  // Initialize TinyUSB
  tusb_init();

  // Wait for USB to be ready
  while (!tud_ready()) {}

  // Set up serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
}

void loop() {
  Serial.println("Hello World");
  delay(1000);

  // Process any pending USB events
  tud_task();
}