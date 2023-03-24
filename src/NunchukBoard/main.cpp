#include <Arduino.h>
#include <Common.h>
#include <EspComms.h>
#include <Nunchuk.h>

Nunchuk nunchuk = Nunchuk();
uint8_t sda = 19;
uint8_t scl = 20;
uint8_t tvcIP = 88;
bool enabled = false;

Comms::Packet pos = {.id = 1, .len=0};
uint32_t updatePos() {
    nunchuk.update();
    pos.len = 0;
    Comms::packetAddUint32(&pos, nunchuk.analogX);
    Comms::packetAddUint32(&pos, nunchuk.analogY);
    if (enabled && nunchuk.zButton){
        Comms::emitPacketToAll(&pos); //actually sends to TVC IP
    }
    Comms::packetAddUint8(&pos, nunchuk.zButton);
    Comms::packetAddUint8(&pos, enabled);
    Comms::emitPacketToGS(&pos);
    return 100*1000; // don't do faster than 10ms
}

uint32_t printPos(){
    Serial.print("Enabled: ");
    Serial.println(enabled);
    Serial.print(" X: ");
    Serial.print(nunchuk.analogX);
    Serial.print(" Y: ");
    Serial.println(nunchuk.analogY);
    Serial.print(" Z button: ");
    Serial.println(nunchuk.zButton);
    return 500*1000;
}

//i used this to figure out what pins were sda/scl lol
/* uint8_t pins[] = {14, 15, 16, 17, 18, 19, 20, 21, 26, 47, 33, 34};
uint32_t readPins(){
    for (int i = 0; i < 12; i++){
      if (digitalRead(pins[i]) == 1){
        Serial.print("Pin ");
        Serial.print(pins[i]);
        Serial.println(" is HIGH");
      }
    }
  return 100*1000;
} */

Task taskTable[] = {
    {updatePos, 1000, true},
    {printPos, 1000, true},
    //{readPins, 0, true}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
  Comms::init();
  Serial.begin(921600);
  nunchuk.init(sda, scl);
  /* for (int i = 0; i < 12; i++){
    pinMode(pins[i], INPUT);
  }
 */
  Comms::redirectEmitPacketToAll(tvcIP);

  Comms::registerCallback(100, [](Comms::Packet packet, uint8_t ip) {
    enabled = Comms::packetGetUint8(&packet, 0);
  });
  

  while(1) {
    // main loop here to avoid arduino overhead
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      uint32_t ticks = micros(); // current time in microseconds
      if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
        uint32_t delayoftask = taskTable[i].taskCall();
        if (delayoftask == 0) {
          taskTable[i].enabled = false;
        }
        else {
          taskTable[i].nexttime = ticks + delayoftask;
        }
      }
    }
    Comms::processWaitingPackets();
  }
}

void loop() {} // unused

