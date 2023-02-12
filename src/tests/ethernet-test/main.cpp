#include <Arduino.h>
#include <SPI.h>
#include <EthernetUdp.h>
#include <Ethernet.h>
#include <EspComms.h>

int count;
EthernetUDP Udp;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 21};
IPAddress groundStation1(10, 0, 0, 70);
IPAddress ip(10, 0, 0, 42);
int port = 42069;
char packetBuffer[sizeof(Comms::Packet)];

void setup()
{
  Serial.begin(921600);
  Ethernet.init(10);
  Ethernet.begin((uint8_t *)mac, ip);
  pinMode(9, INPUT);
  Udp.begin(port);
  Serial.println("running setup");
  Udp.beginPacket(groundStation1, port);
}

void loop()
{
  while(1) {
    Udp.resetSendOffset();
    // Read IO9
    if (Ethernet.detectRead()) {
      if (Udp.parsePacket()) {
        float bruh = micros();
        Udp.read(packetBuffer, sizeof(Comms::Packet));
        Serial.printf("Recieved: %s delta: %d \n", packetBuffer, bruh - micros());
      }
    }
    
  }
}