#include <Arduino.h>
#include <SPI.h>
#include <EthernetUdp.h>
#include <Ethernet.h>

int count;
EthernetUDP Udp;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 21};
IPAddress groundStation1(192, 168, 0, 70);
IPAddress ip(192, 168, 0, 42);
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
  // Serial.println("before begin");
  // Serial.flush();
  while(1) {
    Udp.resetSendOffset();

    char tosend[] = "itimelckdatadatadatadata";

    Udp.write((unsigned char *) tosend, 24);
    
    Serial.println("Writing");
    Udp.endPacket();

    // Read IO9
    uint8_t ACK = digitalRead(9);
    //ACK will be pulled
    Serial.printf("Read Information %i \n", ACK);
    delay(1000);
  }

  
  // Serial.flush();
  // count += 1;

  // delay(1000);
}