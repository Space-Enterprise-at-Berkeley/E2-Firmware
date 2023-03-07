#include "TVC.h"
#include "EEPROM.h"

#define X_PWM_PIN 6

namespace TVC {

    // define update period
    uint32_t tvcUpdatePeriod = 50 * 1000;
    // define class variables    

    void init() {
        //what are you running once?, nothing now 
        pinMode(OUTPUT, X_PWM_PIN);   
        analogWriteFrequency(X_PWM_PIN, 50);
        analogWriteResolution(12);
    }

    uint32_t updatePID() {
        analogWrite(X_PWM_PIN, 655);
        Serial.println("Running PWM");
        return tvcUpdatePeriod;
    }
}
