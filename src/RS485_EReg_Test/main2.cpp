
#include <Arduino.h>

#include <EspComms.h>
#include <Common.h>

char ers485Buffer[sizeof(Comms::Packet)];
char ers485Buffer2[sizeof(Comms::Packet)];
int cnt = 0;
int indicatorDuty = 500;
int indicatorPeriod = 1000;
int indicatorLastTime = 0;


const int timeBetweenTransmission = 100; // ms
int lastTransmissionTime = 0;

const long interval = 25;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.setPins(17, 18);

  for (int i = 0; i < sizeof(Comms::Packet); i++) {
        ers485Buffer2[i] = 82;
    }
}
// RECEIVES R

unsigned long previousMillis = 0;

// Comms::Packet eregPacket = {.id = 23};

void loop()
{

  if(millis() - lastTransmissionTime >= timeBetweenTransmission) {
    DEBUG("Transmitting ");
    // DEBUG(Comms::packetGetFloat(&eregPacket, 0));
    DEBUG("\n");
    DEBUG_FLUSH();
    lastTransmissionTime = lastTransmissionTime + timeBetweenTransmission;
    // Serial1.write(eregPacket.id);
    // Serial1.write(eregPacket.len);
    // Serial1.write(eregPacket.timestamp, 4);
    // Serial1.write(eregPacket.checksum, 2);
    // Serial1.write(eregPacket.data, eregPacket.len);
    Serial2.write('\n');
    Serial2.flush();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // eregPacket.len = 0;
    // Comms::packetAddFloat(&eregPacket, 4.2069);
    
    // uint32_t timestamp = millis();
    // eregPacket.timestamp[0] = timestamp & 0xFF;
    // eregPacket.timestamp[1] = (timestamp >> 8) & 0xFF;
    // eregPacket.timestamp[2] = (timestamp >> 16) & 0xFF;
    // eregPacket.timestamp[3] = (timestamp >> 24) & 0xFF;

    // uint16_t checksum = Comms::computePacketChecksum(&eregPacket);
    // eregPacket.checksum[0] = checksum & 0xFF;
    // eregPacket.checksum[1] = checksum >> 8;
    if (Serial2.available() && cnt < 256) {
        Serial.println("Serial2 available, esp side");
        ers485Buffer[cnt] = Serial2.read();
        Serial.print("printing from teensy --> ereg?: ");
        Serial.println(ers485Buffer[cnt]);
        Serial2.write(ers485Buffer2, cnt);
        cnt++;
    }
    if(cnt == 256) {
            Serial.println("RESETTING BUFFER\n");
            cnt = 0;
        }
  }
  delay(200);
}