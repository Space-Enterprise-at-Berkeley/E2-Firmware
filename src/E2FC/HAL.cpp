#include "HAL.h"

namespace HAL {

    SFE_UBLOX_GNSS neom9n;

    void initHAL() {
        Wire.begin();
        Wire.setClock(100000);

        if(!neom9n.begin(SPI, gpsCSPin, 2000000)) {
            DEBUG("GPS DIDN'T INIT");
            DEBUG("\n");
        } else {
            DEBUG("GPS INIT SUCCESSFUL");
            DEBUG("\n");
        }
    }
}