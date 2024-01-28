#include <RS422Comms.h>


namespace RS422
{

  char rs422Buffer[sizeof(Comms::Packet)];
  int cnt;

  void init(int rxpin, int txpin) {
    RS422_SERIAL.begin(115200, SERIAL_8N1, rxpin, txpin);
    cnt = 0;
  }

  void processAvailableData() {
    while (RS422_SERIAL.available()) {
        rs422Buffer[cnt] = Serial1.read();

        if(rs422Buffer[cnt] == '\n') {
            Comms::Packet *packet = (Comms::Packet *)&rs422Buffer;
            if(Comms::verifyPacket(packet)) {
                cnt = 0;
                //invoke callback
                Serial.println(packet->id);
                Comms::evokeCallbackFunction(packet, FC);
                break;
            }
        }
        cnt++;
    }
  }

  /**
   * @brief Sends packet data over serial.
   *
   * @param packet The packet in which the data is stored.
   */
  void emitPacket(Comms::Packet *packet) {
    emitPacket(packet, &RS422_SERIAL, "\n", 1);
  }

  void emitPacket(Comms::Packet *packet, HardwareSerial *serialBus) {
    emitPacket(packet, serialBus, "\n", 1);
  }

  void emitPacket(Comms::Packet *packet, HardwareSerial *serialBus, char *delim, int dlen)
  {
    Comms::finishPacket(packet);
    // Send over serial
    serialBus->write(packet->id);
    serialBus->write(packet->len);
    serialBus->write(packet->timestamp, 4);
    serialBus->write(packet->checksum, 2);
    serialBus->write(packet->data, packet->len);
    serialBus->write(delim, dlen);
  }

  void sendAbort(uint8_t systemMode, uint8_t abortReason) {
    Comms::Packet packet = {.id = ABORT, .len = 0};
    Comms::packetAddUint8(&packet, systemMode);
    Comms::packetAddUint8(&packet, abortReason);
    RS422::emitPacket(&packet);
  }
};
