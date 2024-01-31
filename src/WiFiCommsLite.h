#pragma once

#include <Common.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "EspComms.h"

#include <Arduino.h>

#include <map>
#include <vector>

namespace WiFiComms
{



  void setSubnetMask(uint8_t one, uint8_t two, uint8_t three, uint8_t four);
  void setIPStart(uint8_t one, uint8_t two, uint8_t three);

  void init();
  void processWaitingPackets();

  void emitPacket(Comms::Packet *packet, uint8_t ip, uint16_t port);

  /**
   * @brief Sends packet data over ethernet and serial towards a specific ip labeled socketNum
   *  Basically a refactoring of the earlier function.
   * @param packet 
   * @param socketNum 
   */
  void emitPacketToGS(Comms::Packet *packet);

  // Broadcast
  void emitPacketToAll(Comms::Packet *packet);
  bool checkAvailable();
};