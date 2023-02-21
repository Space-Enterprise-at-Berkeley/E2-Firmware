#include "AC.h"
#include "MAX22201.h"
#include <EspComms.h>
#include "ChannelMonitor.h"

// PACKET DEFINITIONS FROM SPREADSHEET

// PACKET ID : 2, Actuator States
// 0: Retracting
// 1: Extending 
// 2: Off

// PACKET ID : 100, Actuate Actuator
// 0: Retract fully 
// 1: Extend fully 
// 2: Timed retract 
// 3: Timed extend
// 4: On 
// 5: Off

// 0 and 2 correspond to actuator state 0
// 1 and 3 and 4 correspond to actuator state 1
// 5 corresponds to actuator state 5


// current threshold for a fully extended actuator (i.e less that 0.1A means an actuator has hit its limit switch)
float FULLY_EXTENDED_CURRENT = 0.1;


namespace AC {

// configure actuator driving pins here, in1, in2 from channels 1 to 8
  uint8_t actuatorPins[16] = 
  {
      36, 37,
      6, 7,
      8, 14,
      15, 16,
      17, 18,
      19, 20,
      21, 38,
      39, 40
  };

  // list of driver objects used to actuate each actuator
  MAX22201 actuators[8];


  // called when an actuation needs to begin, registered callback in init
  void beginActuation(Comms::Packet tmp, uint8_t ip) {
    uint8_t channel = packetGetUint8(&tmp, 0);
    uint8_t cmd = packetGetUint8(&tmp, 1);
    uint32_t time = packetGetUint32(&tmp, 2);

    // set states and timers of actuator
    actuators[channel].state = cmd;
    actuators[channel].timeLeft = time;
    actuators[channel].stamp = millis();

    // start actuations based on command received
    if (cmd == 0 || cmd == 2) {
        actuators[channel].backwards();
    }
    else if (cmd == 1 || cmd == 3 || cmd == 4) {
      actuators[channel].forwards();
    }
    else {
      actuators[channel].stop();
    }
    return;
  }

  void init() {
    // Initialise every actuator channel, default state is 0
    for (int i = 0; i < 8; i++) {
      actuators[i].init(actuatorPins[2*i], actuatorPins[2*i+1]);
    }
    // Register the actuation task callback to packet 100
    Comms::registerCallback(100, beginActuation);
  }

  // Daemon task which should be run frequently
  // responsible for stopping all actuations - full actuations are stopped via current sense
  // - our linear actuators have in built limit switches, so pull close to 0 current when full extended either way
  // partial actuations are stopped via checking the current time against when the actuation started
  uint32_t actuationDaemon() {
    for (int i = 0; i < 8; i ++) {
      MAX22201 actuator = actuators[i];
      unsigned long t = millis();

      // if a full actuation, check current
      if (actuator.state == 0 || actuator.state == 1) {
        float* currents = ChannelMonitor::getCurrents();
        if (currents[i] < FULLY_EXTENDED_CURRENT) {
          actuator.stop();
        }
      }
      // if a timed actuation, check times
      else if (actuator.state == 2 || actuator.state == 3) {
        if (t - actuator.stamp > actuator.timeLeft) {
          actuator.timeLeft = 0;
          // stop sets the state to 5 - off
          actuator.stop();
        }
      }
      // don't need to touch anything for commands 4 and 5 - actuator stays on/off
    }

    // run every 10ms, could maybe less time as this is essentially the timing resolution of an actuation
    return 1000 * 10;
  }

  // converts command from packet 100 to actuator state in packet 2
  uint8_t formatActuatorState(uint8_t state) {
    uint8_t mapping[6] = {0, 1, 0, 1, 1, 2};
    return mapping[state];
  }

  // gets every actuator state, formats it, and emits a packet
  uint32_t actuatorStatesTask() {
    Comms::Packet acStates = {.id = 2};
    for (int i = 0; i < 8; i++) {
      packetAddUint8(&acStates, formatActuatorState(actuators[i].state));
    }
    Comms::emitPacket(&acStates);
    return 1000 * 1000;
  }

}