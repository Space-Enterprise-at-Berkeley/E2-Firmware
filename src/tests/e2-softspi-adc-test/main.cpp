#include <Arduino.h>
#include <SPI.h>

#include <ADS8167.h>

ADS8167 adc1;

SoftSPI spi2Obj(41,42,45);
SoftSPI* spi2 = &spi2Obj;


void setup(){

    Serial.begin(921600);
    spi2->begin();
    spi2->setClockDivider(SPI_CLOCK_DIV64);
    spi2->setDataMode(SPI_MODE0);

    // spi2->begin(45, 42, 41, 40);
    adc1.init(spi2, 40, 46);
    adc1.setAllInputsSeparate();
    adc1.enableOTFMode();


    while(1) {
        Serial.print("adc10: ");
        Serial.print(adc1.readData(0));
        Serial.print("   adc11: ");
        Serial.print(adc1.readData(1));
        Serial.print("   adc12: ");
        Serial.print(adc1.readData(2));
        Serial.print("   adc13: ");
        Serial.print(adc1.readData(3));
        Serial.print("   adc14: ");
        Serial.print(adc1.readData(4));
        Serial.print("   adc15: ");
        Serial.print(adc1.readData(5));
        Serial.print("   adc16: ");
        Serial.print(adc1.readData(6));
        Serial.print("   adc17: ");
        Serial.println(adc1.readData(7));

    }

    //on the fly testing
    /*
    while(1){
        Serial.print("adc10: ");
        Serial.print(adc1.readc1hannelOTF(1));
        Serial.print("   adc11: ");
        Serial.print(adc1.readc1hannelOTF(2));
        Serial.print("   adc12: ");
        Serial.print(adc1.readc1hannelOTF(3));
        Serial.print("   adc13: ");
        Serial.print(adc1.readc1hannelOTF(4));
        Serial.print("   adc14: ");
        Serial.print(adc1.readc1hannelOTF(5));
        Serial.print("   adc15: ");
        Serial.print(adc1.readc1hannelOTF(6));
        Serial.print("   adc16: ");
        Serial.print(adc1.readc1hannelOTF(7));
        Serial.print("   adc17: ");
        Serial.println(adc1.readc1hannelOTF(0));
        delay(2000);
    }
    */
    return;

}

void loop() {
    // unused
}