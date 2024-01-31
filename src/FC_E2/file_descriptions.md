Organization:

The main.cpp file is the jumping off place for all code. In an effort to keep code neat, everything the FC does is segregated as "tasks" and "callback functions" in other files, which is why the main.cpp file has very little actual code.

Actuators.cpp: all the code that allows you to actuate things, plus callbacks that listen for the actuate command and do it.

Automation.cpp: actuates things that aren't directly commanded. Handles launch cmd, GEMS autovent, aborts via callbacks.

ChannelMoniter.cpp: Helper abstraction layer for actuator stuff. Handles actuator current and continuity sensing.

Ducers.cpp: all the code for reading and sending PT data, plus callbacks to calibrate the PTs.

EReg.cpp: RS422 comms code to forward packets from GS to EREG, and vice versa. (Cursed packet id remapping present)

FlightSensors.cpp: all the code for reading and sending IMUs, barometers, and GPS.