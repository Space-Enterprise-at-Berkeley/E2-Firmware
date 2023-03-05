#include <EspComms.h>

namespace Comms {
  std::map<uint8_t, commFunction> callbackMap;

  EthernetUDP Udp;
  char packetBuffer[sizeof(Packet)];

  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, IP_ADDRESS_END};
  IPAddress groundStation1(10, 0, 0, 169);
  IPAddress groundStation2(10, 0, 0, 170);
  IPAddress ip(10, 0, 0, IP_ADDRESS_END);
  int port = 42069;

  void init(int cs, int spiMisoPin, int spiMosiPin, int spiSclkPin, int ETH_intN)
  {
    Ethernet.init(cs);
    Ethernet.begin((uint8_t *)mac, ip, spiMisoPin, spiMosiPin, spiSclkPin, ETH_intN);
    Udp.begin(port);

    Udp.beginPacket(groundStation1, port);
    Udp.beginPacket(groundStation2, port);
  }

  void init() {
    init(10, -1, -1, -1, -1);
  }

  void sendFirmwareVersionPacket(Packet unused, uint8_t ip)
  {
    DEBUG("sending firmware version packet\n");
    DEBUG_FLUSH();

    Packet version = {.id = 0, .len = 7};

    char commit[] = FW_COMMIT;
    memcpy(&(version.data), &commit, 7);
    emitPacket(&version);
  }

  void registerCallback(uint8_t id, commFunction function)
  {
    callbackMap.insert(std::pair<int, commFunction>(id, function));
  }

  /**
   * @brief Checks checksum of packet and tries to call the associated callback function.
   *
   * @param packet Packet to be processed.
   * @param ip End byte in IP address of sender (255 if usb packet)
   */
  void evokeCallbackFunction(Packet *packet, uint8_t ip)
  {
    uint16_t checksum = *(uint16_t *)&packet->checksum;
    if (checksum == computePacketChecksum(packet))
    {
      // DEBUG("Packet with ID ");
      // DEBUG(packet->id);
      // DEBUG(" has correct checksum!\n");
      // try to access function, checking for out of range exception
      if (callbackMap.count(packet->id))
      {
        callbackMap.at(packet->id)(*packet, ip);
      }
      else
      {
        // DEBUG("ID ");
        // DEBUG(packet->id);
        // DEBUG(" does not have a registered callback function.\n");
      }
    }
    else
    {
      DEBUG("Packet with ID ");
      DEBUG(packet->id);
      DEBUG(" does not have correct checksum!\n");
    }
  }

  void processWaitingPackets()
  {
    Udp.resetSendOffset();
    if (Ethernet.detectRead()) {
      if (Udp.parsePacket()) {
        if(Udp.remotePort() != port) return;
        Udp.read(packetBuffer, sizeof(Comms::Packet));
        Packet *packet = (Packet*) &packetBuffer;
        evokeCallbackFunction(packet, Udp.remoteIP()[3]);
      }
    }

    if (Serial.available())
      {
        int cnt = 0;
        while (Serial.available() && cnt < sizeof(Packet))
        {
          packetBuffer[cnt] = Serial.read();
          cnt++;
        }
        Packet *packet = (Packet *)&packetBuffer;
        // DEBUG("Got unverified packet with ID ");
        // DEBUG(packet->id);
        // DEBUG('\n');
        evokeCallbackFunction(packet, 255); // 255 signifies a USB packet
      }
  }

  void packetAddFloat(Packet *packet, float value)
  {
    uint32_t rawData = *(uint32_t *)&value;
    packet->data[packet->len] = rawData & 0xFF;
    packet->data[packet->len + 1] = rawData >> 8 & 0xFF;
    packet->data[packet->len + 2] = rawData >> 16 & 0xFF;
    packet->data[packet->len + 3] = rawData >> 24 & 0xFF;
    packet->len += 4;
  }

  void packetAddUint32(Packet *packet, uint32_t value)
  {
    packet->data[packet->len] = value & 0xFF;
    packet->data[packet->len + 1] = value >> 8 & 0xFF;
    packet->data[packet->len + 2] = value >> 16 & 0xFF;
    packet->data[packet->len + 3] = value >> 24 & 0xFF;
    packet->len += 4;
  }

  void packetAddUint16(Packet *packet, uint16_t value)
  {
    packet->data[packet->len] = value & 0xFF;
    packet->data[packet->len + 1] = value >> 8 & 0xFF;
    packet->len += 2;
  }

  void packetAddUint8(Packet *packet, uint8_t value)
  {
    packet->data[packet->len] = value;
    packet->len++;
  }

  float packetGetFloat(Packet *packet, uint8_t index)
  {
    uint32_t rawData = packet->data[index + 3];
    rawData <<= 8;
    rawData += packet->data[index + 2];
    rawData <<= 8;
    rawData += packet->data[index + 1];
    rawData <<= 8;
    rawData += packet->data[index];
    return *(float *)&rawData;
  }

  uint32_t packetGetUint32(Packet *packet, uint8_t index)
  {
    uint32_t rawData = packet->data[index + 3];
    rawData <<= 8;
    rawData += packet->data[index + 2];
    rawData <<= 8;
    rawData += packet->data[index + 1];
    rawData <<= 8;
    rawData += packet->data[index];
    return rawData;
  }

  uint32_t packetGetUint8(Packet *packet, uint8_t index)
  {
    return packet->data[index];
  }

  /**
   * @brief Sends packet to both groundstations.
   *
   * @param packet Packet to be sent.
   */
  void emitPacket(Packet *packet)
  {
    // add timestamp to struct
    uint32_t timestamp = millis();
    packet->timestamp[0] = timestamp & 0xFF;
    packet->timestamp[1] = (timestamp >> 8) & 0xFF;
    packet->timestamp[2] = (timestamp >> 16) & 0xFF;
    packet->timestamp[3] = (timestamp >> 24) & 0xFF;

    // calculate and append checksum to struct
    uint16_t checksum = computePacketChecksum(packet);
    packet->checksum[0] = checksum & 0xFF;
    packet->checksum[1] = checksum >> 8;



    // Send over UDP
    // Udp.resetSendOffset();
    Udp.beginPacket(groundStation1, port);
    Udp.write(packet->id);
    Udp.write(packet->len);
    Udp.write(packet->timestamp, 4);
    Udp.write(packet->checksum, 2);
    Udp.write(packet->data, packet->len);
    Udp.endPacket();

    Udp.beginPacket(groundStation2, port);
    Udp.write(packet->id);
    Udp.write(packet->len);
    Udp.write(packet->timestamp, 4);
    Udp.write(packet->checksum, 2);
    Udp.write(packet->data, packet->len);
    Udp.endPacket();
  }

  void emitPacket(Packet *packet, uint8_t ip)
  {
    // add timestamp to struct
    uint32_t timestamp = millis();
    packet->timestamp[0] = timestamp & 0xFF;
    packet->timestamp[1] = (timestamp >> 8) & 0xFF;
    packet->timestamp[2] = (timestamp >> 16) & 0xFF;
    packet->timestamp[3] = (timestamp >> 24) & 0xFF;

    // calculate and append checksum to struct
    uint16_t checksum = computePacketChecksum(packet);
    packet->checksum[0] = checksum & 0xFF;
    packet->checksum[1] = checksum >> 8;



    // Send over UDP
    // Udp.resetSendOffset();
    Udp.beginPacket(IPAddress(10,0,0,ip), port);
    Udp.write(packet->id);
    Udp.write(packet->len);
    Udp.write(packet->timestamp, 4);
    Udp.write(packet->checksum, 2);
    Udp.write(packet->data, packet->len);
    Udp.endPacket();
  }



  bool verifyPacket(Packet *packet)
  {
    uint16_t csum = computePacketChecksum(packet);
    return ((uint8_t)csum & 0xFF) == packet->checksum[0] && ((uint8_t)(csum >> 8)) == packet->checksum[1];
  }

  /**
   * @brief generates a 2 byte checksum from the information of a packet
   *
   * @param data pointer to data array
   * @param len length of data array
   * @return uint16_t
   */
  uint16_t computePacketChecksum(Packet *packet)
  {

    uint8_t sum1 = 0;
    uint8_t sum2 = 0;

    sum1 = sum1 + packet->id;
    sum2 = sum2 + sum1;
    sum1 = sum1 + packet->len;
    sum2 = sum2 + sum1;

    for (uint8_t index = 0; index < 4; index++)
    {
      sum1 = sum1 + packet->timestamp[index];
      sum2 = sum2 + sum1;
    }

    for (uint8_t index = 0; index < packet->len; index++)
    {
      sum1 = sum1 + packet->data[index];
      sum2 = sum2 + sum1;
    }
    return (((uint16_t)sum2) << 8) | (uint16_t)sum1;
  }
};
