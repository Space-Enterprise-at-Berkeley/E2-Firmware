#include <Arduino.h>

void setup() 
{
  Serial.begin(921600);
}

void loop()
{
    Serial.println("Hello World!");
    delay(1000);
}