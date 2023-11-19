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

#define initWire() Wire.setClock(400000); Wire.setPins(HAL::SDA,HAL::SCL); Wire.begin()

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

  LOX_EREG = 61,
  FUEL_EREG = 62,
  EREG3 = 63,

  FC = 42,
  
  GROUND1 = 169,
  GROUND2 = 170,
  GROUND3 = 171,

  LOX_INJ_EREG = 28,
  FUEL_INJ_EREG = 27,

  ALL = 255,
};

//Define Packet ID Enum
enum PacketID {
  // Common
  FW_STATUS = 0,

  //BY FC
  PWR_DATA = 1,
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
  FC_TO_EREG = 25,
  // TO FC
  FC_FAST_READ_RATE = 1,
  FC_LOX_GEMS = 2,
  FC_FUEL_GEMS = 3,
  FC_TOGGLE_LOX = 4,
  FC_TOGGLE_FUEL = 5,
  FC_PRESS_FLOW_RBV = 6,

  // BY EREG
  EREG_TELEMETRY = 1,
  EREG_MOTOR = 2,
  EREG_CONFIG = 3,
  EREG_DIAGNOSTIC = 4,
  EREG_ERROR_STATE = 5,
  EREG_FLOW_STATE = 6,
  EREG_LIMIT_SWITCH = 7,
  EREG_PHASE_CURRENTS = 8,
  EREG_TEMP_DATA = 9,
  EREG_OVERCURRENT = 11,
  EREG_ABORT = 12,
  EREG_PTCALIBRATION = 102,
  // TO EREG
  EREG_BEGIN = 200,
  EREG_CLOSE = 201,
  EREG_PARTIAL = 202,
  EREG_STATIC = 203,
  EREG_DIAG_CHECKOUT = 204,
  EREG_ZERO = 205,
  EREG_P = 206,
  EREG_I = 207,
  EREG_D = 208,
  EREG_PING = 209,
  EREG_ACTUATE_MAIN = 210,
  ZERO_CMD = 100,
  CAL_CMD = 101,
  SEND_CAL = 102,
  RESET_CAL = 103,

  // MISC
  HEARTBEAT = 250,
  MODE_CMD = 251, // ? not in packet definitions
  ABORT = 133,
  LAUNCH_QUEUE = 149,
  STARTFLOW = 150,
  ENDFLOW = 151,

  // BY PT
  // same pwr data packet num as fc
  PT_DATA = 2,
  // TO LC / PT
  // same first point as ereg
  // same second point as ereg
  // same send calibration as ereg
  // same reset calibration as ereg

  // BY TC
  // same pwr data packet num as fc
  TC_DATA = 2,

  // BY LC
  // same pwr data packet num as fc
  LC_DATA = 2,
  // same send calibration as ereg

  // BY AC
  // same pwr data packet num as fc
  AC_STATE = 2,
  AC_CONTINUITY = 3,
  AC_CURRENT = 4,
  AC_CONFIG = 5,
  AC_ENCODER = 6,
  // TO AC
  ACTUATE_CMD = 100,
  AC_CHANGE_CONFIG = 105,

  // GEMS autovent
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