#include <Arduino.h>
#include <SPI.h>
#include <EthernetUdp.h>
#include <Ethernet.h>

int count;
EthernetUDP Udp;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 21};
IPAddress groundStation1(10, 0, 0, 70);
IPAddress ip(10, 0, 0, 42);
int port = 42069;

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
    uint8_t ACK = digitalRead(9);
    //ACK will be pulled
    
    Serial.printf("%i \n", ACK);
    if (ACK == 0) {
      Serial.print("ACK \n");
      // pin is pulled down. Manually reset it.
      Ethernet.resetINTnPin(); // Done with this custom method
    } else {
      Serial.print("FAKE \n");
    }
    sleep(0.5);
  }
}