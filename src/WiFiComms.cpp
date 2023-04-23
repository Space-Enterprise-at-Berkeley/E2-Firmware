#include <WiFiComms.h>

namespace Comms {
  std::map<uint8_t, commFunction> callbackMap;

  WiFiUdp Udp;
  char packetBuffer[sizeof(Packet)];

  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, ID};
  uint8_t ip_start[] = {10, 0, 0};
  IPAddress subnet = IPAddress(255, 255, 255, 0);
  IPAddress gateway = IPAddress(ip_start[0], ip_start[1], ip_start[2], 1);

// TODO: multiple ports. 
//   const uint8_t groundStationCount = 3;
//   IPAddress groundStations[groundStationCount] = {IPAddress(ip_start[0], ip_start[1], ip_start[2], GROUND1), IPAddress(ip_start[0], ip_start[1], ip_start[2], GROUND2), IPAddress(ip_start[0], ip_start[1], ip_start[2], GROUND3)};
//   int ports[groundStationCount] = {42069, 42070, 42071};

  IPAddress groundStations[groundStationCount] = {IPAddress(10, 0, 0, GROUND1)};
  int ports[groundStationCount] = {42069};

  IPAddress ip(ip_start[0], ip_start[1], ip_start[2], ID);

  void setSubnet(uint8_t one, uint8_t two, uint8_t three, uint8_t four) {
    subnet = IPAddress(one, two, three, four);
  }
  void setIPStart(uint8_t one, uint8_t two, uint8_t three) {
    ip_start[0] = one;
    ip_start[1] = two;
    ip_start[2] = three;
    gateway = IPAddress(ip_start[0], ip_start[1], ip_start[2], 1);

    for (int i = 0; i < groundStationCount; i++) {
      groundStations[i] = IPAddress(ip_start[0], ip_start[1], ip_start[2], GROUND1 + i);
    }

    ip = IPAddress(ip_start[0], ip_start[1], ip_start[2], IP_ADDRESS_END);
    WiFi.config(ip, gateway, subnet);
  }

  void init()
  {
    Serial.begin(926100);

    char ssid[] = NET_SSID;
    char password[] = NET_PASSWORD;
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);

    WiFi.setAutoReconnect(true);

    //TODO: multiple ports
    Udp.begin(ports[0]);
  }

void sendFirmwareVersionPacket(Packet unused, uint8_t ip)
  {
    DEBUG("sending firmware version packet\n");
    DEBUG_FLUSH();

    Packet version = {.id = FW_STATUS, .len = 7};

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
      Serial.print("Packet with ID ");
      Serial.print(packet->id);
      Serial.print(" has correct checksum!\n");
      // try to access function, checking for out of range exception
      if (callbackMap.count(packet->id))
      {
        callbackMap.at(packet->id)(*packet, ip);
      }
      else
      {
        Serial.print("ID ");
        Serial.print(packet->id);
        Serial.print(" does not have a registered callback function.\n");
      }
    }
    else
    {
      Serial.print("Packet with ID ");
      Serial.print(packet->id);
      Serial.print(" does not have correct checksum!\n");
    }
  }

  void processWaitingPackets()
  {
    if (Udp.available()) {
      if (Udp.parsePacket()) {
        // if(Udp.remotePort() != port) return;
        Udp.read(packetBuffer, sizeof(Comms::Packet));
        Packet *packet = (Packet*) &packetBuffer;
        evokeCallbackFunction(packet, Udp.remoteIP()[3]);
        
      }
    }

    if (Serial.available())
      {
        //That was for reading full formed packets from the USB serial port
        /*
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
        */
       
       //Instead I want to read commands in the form of "id data"
       //And then make the packet and trigger the callback

        Serial.println("Got a command");
        uint8_t id = (uint8_t)Serial.parseInt();
        Serial.print("id" + String(id));
        if (id == -1) return;
        Packet packet = {.id = id, .len = 0};
        while(Serial.available()){
          if (Serial.peek() == ' ') Serial.read();
          if (Serial.peek() == '\n') {Serial.read(); break;}
          //determine datatype of next value
          if (Serial.peek() == 'f'){
            Serial.read();
            float val = Serial.parseFloat();
            Serial.print(" float" + String(val));
            packetAddFloat(&packet, val);
          }
          else if (Serial.peek() == 'i'){
            Serial.read();
            int val = Serial.parseInt();
            Serial.print(" int" + String(val));
            packetAddUint32(&packet, val);
          }
          else if (Serial.peek() == 's'){
            Serial.read();
            int val = Serial.parseInt();
            Serial.print(" short" + String(val));
            packetAddUint16(&packet, val);
          }
          else if (Serial.peek() == 'b'){
            Serial.read();
            int val = Serial.parseInt();
            Serial.print(" byte" + String(val));
            packetAddUint8(&packet, val);
          } else{
            Serial.read();
          }
        }
        Serial.println();
            // add timestamp to struct
        uint32_t timestamp = millis();
        packet.timestamp[0] = timestamp & 0xFF;
        packet.timestamp[1] = (timestamp >> 8) & 0xFF;
        packet.timestamp[2] = (timestamp >> 16) & 0xFF;
        packet.timestamp[3] = (timestamp >> 24) & 0xFF;

        // calculate and append checksum to struct
        uint16_t checksum = computePacketChecksum(&packet);
        packet.checksum[0] = checksum & 0xFF;
        packet.checksum[1] = checksum >> 8;
        evokeCallbackFunction(&packet, 255); // 255 signifies a USB packet
      }
  }

  void packetAddFloatArray(Packet *packet, float *arr, uint8_t len)
  {
    for (int i = 0; i < len; i ++){
      packetAddFloat(packet, *(arr + i));
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

  void finishPacket(Packet *packet){
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

  }

  void emitPacketToGS(Packet *packet)
  {
    finishPacket(packet);

    // Send over UDP
    // Udp.resetSendOffset();
    for (int i = 0; i < groundStationCount; i++){
      Udp.beginPacket(groundStations[i], ports[i]);
      Udp.write(packet->id);
      Udp.write(packet->len);
      Udp.write(packet->timestamp, 4);
      Udp.write(packet->checksum, 2);
      Udp.write(packet->data, packet->len);
      Udp.endPacket(i+1);
    }
  }

  void emitPacketToAll(Packet *packet)
  {
    finishPacket(packet);

    // Send over UDP
    // Udp.resetSendOffset();
    Udp.beginPacket(ALL, 42099)
    Udp.write(packet->id);
    Udp.write(packet->len);
    Udp.write(packet->timestamp, 4);
    Udp.write(packet->checksum, 2);
    Udp.write(packet->data, packet->len);
    Udp.endPacket();
  }

  void emitPacket(Packet *packet, uint8_t ip, uint16_t port){
    //Serial.println("Emitting packet to " + String(ip));
    finishPacket(packet);

    // Send over UDP
    // Udp.resetSendOffset();
    Udp.beginPacket(ip, port);
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

  void sendAbort(uint8_t systemMode, uint8_t abortReason){
    Packet packet = {.id = ABORT, .len = 0};
    packetAddUint8(&packet, systemMode);
    packetAddUint8(&packet, abortReason);
    emitPacketToAll(&packet);
    Serial.println("Abort sent, mode " + String((Mode)systemMode) + " reason " + String((AbortReason)abortReason));
  }
};