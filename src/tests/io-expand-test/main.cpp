#include <Arduino.h>
#include <EspComms.h>
#include <MCP23008.h>

MCP23008 MCP2(0x27);

void setup() { 
    // inits IO Expanders, sets all pins as output pins, and turns all LEDs off
    Wire.begin(1, 2);
    MCP2.begin();
    MCP2.pinMode8(0x00);  // 0 = output , 1 = input
    Wire.setClock(100000);
    MCP2.write8(LOW);
}

void loop() { 
    for (int i = 4; i < 5; i ++){
        MCP2.digitalWrite(i, 1);
        delay(2000);
        MCP2.digitalWrite(i, 0);
        delay(1000);
    }   
}