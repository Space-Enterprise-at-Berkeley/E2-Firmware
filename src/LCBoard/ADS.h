#pragma once

#include <Common.h>
#include <EspComms.h>

#include<ADS1231.h>

#include <Arduino.h>



namespace ADS {

    extern const int size;



    void init();

    void refreshReadings();
    uint32_t sampleLC();



}