#include "TVC.h"
#include "EEPROM.h"

#define X_PWM_PIN 0

namespace TVC {

    // define update period
    uint32_t tvcUpdatePeriod = 50 * 1000;
    // define class variables    

    void init() {
        //what are you running once?, nothing now        
    }

    uint32_t updatePID() {
        analogWrite(X_PWM_PIN, 50);
        Serial.println("Running PWM");
        return tvcUpdatePeriod;
    }
}
