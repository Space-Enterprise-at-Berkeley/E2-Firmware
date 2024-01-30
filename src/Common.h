#pragma once

#include <Arduino.h>

#ifdef DEBUG_MODE
#define DEBUG(val) Serial.print(val)
#define DEBUG_FLUSH() Serial.flush()
#define DEBUGLN(val) Serial.println(val)
#else
#define DEBUG(val)
#define DEBUG_FLUSH()
#define DEBUGLN(val)
#endif

struct Task {
    uint32_t (*taskCall)(void);
    uint32_t nexttime;
    bool enabled;
};

#define initWire() Wire.setClock(400000); Wire.setPins(1, 2); Wire.begin()

//Define Board ID Enum
enum BoardID
{
  AC1 = 11,
  AC2 = 12,
  LC1 = 21,
  LC2 = 22,
  PT1 = 31,
  PT2 = 32,
  TC1 = 51,
  TC2 = 52,
  FC = 42,
  LOX_EREG = 61,
  FUEL_EREG = 62,
  EREG3 = 63,
  GROUND1 = 169,
  GROUND2 = 170,
  GROUND3 = 171,
  LOX_INJ_EREG = 28,
  FUEL_INJ_EREG = 27,
  ALL = 255,
};

//Define Packet ID Enum
enum PacketID {
  FW_STATUS = 0,
  HEARTBEAT = 250,
  MODE_CMD = 251,
  PWR_DATA = 1,
  ABORT = 133,
  LAUNCH_QUEUE = 149,
  STARTFLOW = 150,
  ENDFLOW = 151,
  //Does not include FC or EREG
  //PT
  PT_DATA = 2,
  ZERO_CMD = 100,
  CAL_CMD = 101,
  SEND_CAL = 102,
  RESET_CAL = 103,
  //TC
  TC_DATA = 2,
  //LC
  LC_DATA = 2,
  //AC
  AC_STATE = 2,
  AC_CONTINUITY = 3,
  AC_CURRENT = 4,
  AC_CONFIG = 5,
  ACTUATE_CMD = 100,

  //FC
  //PWR_DATA = 1,
  BOARD_STATE = 2,
  FLIGHT_STATE = 3,
  IMU_DATA = 4,
  BARO_DATA = 5,
  GPS_DATA = 6,
  //TO FC
  FC_ACTUATE = 100,
  FC_PT_ZERO = 101,
  FC_PT_CAL = 102,
  FC_PT_SEND_CAL = 103,
  FC_PT_RESET_CAL = 104,

  FC_LOX_PRESSURE = 171,
  FC_FUEL_PRESSURE = 172,

  FC_SET_AUTOVENT = 110,
  FC_SEND_AUTOVENT = 25,

  AC_CHANGE_CONFIG = 105,

  //GEMS autovent
  EREG_PRESSURE = 171,
  
};

//Define Use Mode Enum
enum Mode {
  LAUNCH = 0,
  HOTFIRE = 1,
  COLDFLOW = 2,
  COLDFLOW_WITH_IGNITER = 3,
  GASFLOW = 4,
};

//Define Abort Reason Enum
enum AbortReason {
  TANK_OVERPRESSURE = 0,
  ENGINE_OVERTEMP = 1,
  LC_UNDERTHRUST = 2,
  MANUAL_ABORT = 3,
  IGNITER_NO_CONTINUITY = 4,
  BREAKWIRE_NO_CONTINUITY = 5,
  BREAKWIRE_NO_BURNT = 6,
};

  //BY FC
  /*
  IMU_DATA = 2,
  BARO_DATA = 3,
  GPS_DATA = 4,
  LOX_GEMS_IV = 5,
  FUEL_GEMS_IV = 6,
  LOX_GEMS_STATE = 7,
  FUEL_GEMS_STATE = 8,
  PRESS_FLOW_STATE = 11,
  APOGEE = 12,
  VEHICLE_STATE = 13,
  BLACKBOX_WRITTEN = 14,
  FLIGHT_OC_EVENT = 15,
  AUTOVENT_STATE = 16,
  BREAKWIRE1_STATE = 17,
  BREAKWIRE2_STATE = 18,
  PT_DATA = 19,
  RTD_DATA = 20,
  LOX_CAPFILL_DATA = 21,
  FUEL_CAPFILL_DATA = 22,
  MAIN_CHUTE_DEPLOY = 23,
  DROGUE_CHUTE_DEPLOY = 24,
  //BY EREG
  EREG_TELEMETRY = 1,
  EREG_CONFIG = 2,
  EREG_DIAGNOSTIC = 3,
  EREG_ERROR_STATE = 4,
  EREG_FLOW_STATE =5,
  EREG_LIMIT_SWITCH = 6,
  EREG_PHASE_CURRENTS = 7,
  EREG_TEMP_DATA = 8,
  EREG_ABORT = 9,
  EREG_OVERCURRENT = 10,
  */