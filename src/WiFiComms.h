#pragma once

#include <Common.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <Arduino.h>

#include <map>
#include <vector>

namespace Comms
{

  struct Packet
  {
    uint8_t id;
    uint8_t len;
    uint8_t timestamp[4];
    uint8_t checksum[2];
    uint8_t data[256];
  };

  void setSubnetMask(uint8_t one, uint8_t two, uint8_t three, uint8_t four);
  void setIPStart(uint8_t one, uint8_t two, uint8_t three);

  void init();

  typedef void (*commFunction)(Packet, uint8_t);

  /**
   * @brief Registers methods to be called when Comms receives a packet with a specific ID.
   *
   * @param id The ID of the packet associated with a specific command.
   * @param function a pointer to a method that takes in a Packet struct.
   */
  void registerCallback(uint8_t id, commFunction function);

  void processWaitingPackets();

  void packetAddFloat(Packet *packet, float value);
  void packetAddUint32(Packet *packet, uint32_t value);
  void packetAddUint16(Packet *packet, uint16_t value);
  void packetAddUint8(Packet *packet, uint8_t value);

  /**
   * @brief Interprets the packet data as a float.
   *
   * @param packet
   * @param index The index of the byte array at which the float starts (0, 4, 8).
   * @return float
   */
  float packetGetFloat(Packet *packet, uint8_t index);
  uint32_t packetGetUint32(Packet *packet, uint8_t index);
  uint32_t packetGetUint8(Packet *packet, uint8_t index);

  /**
   * @brief Adds time and checksum to packet.
   *
   * @param packet Packet to be processed.
   */
  void finishPacket(Packet *packet);

  /**
   * @brief Sends packet data over ethernet and serial.
   *
   * @param packet The packet in which the data is stored.
   */
  void emitPacket(Packet *packet, uint8_t ip, uint16_t port);

  /**
   * @brief Sends packet data over ethernet and serial towards a specific ip labeled socketNum
   *  Basically a refactoring of the earlier function.
   * @param packet 
   * @param socketNum 
   */
  void emitPacketToGS(Packet *packet);

  // Broadcast
  void emitPacketToAll(Packet *packet);


  bool verifyPacket(Packet *packet);

  uint16_t computePacketChecksum(Packet *packet);

  /**
   * @brief Sends the firmware version packet upon request
   *
   * @param _ unused
   */
  void sendFirmwareVersionPacket(Packet unused, uint8_t ip);

  /**
   * @brief Broadcasts an abort packet with the current system mode and abort reason.
   *
   * @param systemMode The current system mode. Follows enum in Common.h.
   * @param abortReason The current abort reason. Follows enum in Common.h.
   */
  void sendAbort(uint8_t systemMode, uint8_t abortReason);
};