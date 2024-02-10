#include <Arduino.h>
#include <SPI.h>

#include <ADS8167.h>

ADS8167 adc;
SPIClass *spi2; 

void setup(){
    Serial.begin(921600);

    SoftSPI spi2Obj(41,42,45);
    SoftSPI* spi2 = &spi2Obj;
    
    spi2->begin();
    spi2->setClockDivider(SPI_CLOCK_DIV128);
    spi2->setDataMode(SPI_MODE0);

    // spi2->begin(45, 42, 41, 40);
    adc.init(spi2, 40, 46);
    adc.setAllInputsSeparate();
    adc.enableOTFMode();

    while(1) {
        Serial.print("adc0: ");
        Serial.print(adc.readData(0));
        Serial.print("   adc1: ");
        Serial.print(adc.readData(1));
        Serial.print("   adc2: ");
        Serial.print(adc.readData(2));
        Serial.print("   adc3: ");
        Serial.print(adc.readData(3));
        Serial.print("   adc4: ");
        Serial.print(adc.readData(4));
        Serial.print("   adc5: ");
        Serial.print(adc.readData(5));
        Serial.print("   adc6: ");
        Serial.print(adc.readData(6));
        Serial.print("   adc7: ");
        Serial.println(adc.readData(7));
        delay(200);
    }

    //on the fly testing
    /*
    while(1){
        Serial.print("adc0: ");
        Serial.print(adc.readChannelOTF(1));
        Serial.print("   adc1: ");
        Serial.print(adc.readChannelOTF(2));
        Serial.print("   adc2: ");
        Serial.print(adc.readChannelOTF(3));
        Serial.print("   adc3: ");
        Serial.print(adc.readChannelOTF(4));
        Serial.print("   adc4: ");
        Serial.print(adc.readChannelOTF(5));
        Serial.print("   adc5: ");
        Serial.print(adc.readChannelOTF(6));
        Serial.print("   adc6: ");
        Serial.print(adc.readChannelOTF(7));
        Serial.print("   adc7: ");
        Serial.println(adc.readChannelOTF(0));
        delay(2000);
    }
    */
    return;

}

void loop() {
    // unused
}