#include <WiFiCommsLite.h>

namespace WiFiComms {

  char packetBuffer[sizeof(Comms::Packet)];

  WiFiUDP Udp;

  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, IPADDR};
  uint8_t ip_start[] = {192, 168, 43}; //{10, 0, 0};
  IPAddress subnet = IPAddress(255, 255, 255, 0);
  IPAddress gateway = IPAddress(ip_start[0], ip_start[1], ip_start[2], 1);

// TODO: multiple ports. 
  const uint8_t groundStationCount = 1;
//   IPAddress groundStations[groundStationCount] = {IPAddress(ip_start[0], ip_start[1], ip_start[2], GROUND1), IPAddress(ip_start[0], ip_start[1], ip_start[2], GROUND2), IPAddress(ip_start[0], ip_start[1], ip_start[2], GROUND3)};
//   int ports[groundStationCount] = {42069, 42070, 42071};

  IPAddress groundStations[groundStationCount] = {IPAddress(ip_start[0], ip_start[1], ip_start[2], 69)}; //{IPAddress(10, 0, 0, GROUND1)};
  int ports[groundStationCount] = {42069};

  IPAddress ip(ip_start[0], ip_start[1], ip_start[2], IPADDR);

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

    ip = IPAddress(ip_start[0], ip_start[1], ip_start[2], IPADDR);
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

  void processWaitingPackets()
  {
    if (Udp.available()) {
      if (Udp.parsePacket()) {
        // if(Udp.remotePort() != port) return;
        Udp.read(packetBuffer, sizeof(Comms::Packet));
        Comms::Packet *packet = (Comms::Packet*) &packetBuffer;
        Comms::evokeCallbackFunction(packet, Udp.remoteIP()[3]);
        
      }
    }
  }

  void emitPacketToGS(Comms::Packet *packet)
  {
    Comms::finishPacket(packet);

    // Send over UDP
    // Udp.resetSendOffset();
    for (int i = 0; i < groundStationCount; i++){
      Udp.beginPacket(groundStations[i], ports[i]);
      Udp.write(packet->id);
      Udp.write(packet->len);
      Udp.write(packet->timestamp, 4);
      Udp.write(packet->checksum, 2);
      Udp.write(packet->data, packet->len);
      Udp.endPacket();
    }
  }

  void emitPacketToAll(Comms::Packet *packet)
  {
    Comms::finishPacket(packet);

    // Send over UDP
    // Udp.resetSendOffset();
    Udp.beginPacket(ALL, 42099);
    Udp.write(packet->id);
    Udp.write(packet->len);
    Udp.write(packet->timestamp, 4);
    Udp.write(packet->checksum, 2);
    Udp.write(packet->data, packet->len);
    Udp.endPacket();
  }

  void emitPacket(Comms::Packet *packet, uint8_t ip, uint16_t port){
    //Serial.println("Emitting packet to " + String(ip));
    Comms::finishPacket(packet);

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

  bool checkAvailable() {
    return (Udp.available() && Udp.parsePacket());
  }
};