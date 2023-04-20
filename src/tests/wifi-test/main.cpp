#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <EspComms.h>

WiFiUDP udp;
char packetBuffer[255];
unsigned int localPort = 42069;
//IPAddress groundStation1(192, 168, 1, 169);
IPAddress groundStation1(10, 0, 0, 169);
//IPAddress ip(192, 168, 1, IP_ADDRESS_END);
IPAddress ip(10, 0, 0, IP_ADDRESS_END);
//IPAddress gateway(192, 168, 1, 1);
IPAddress gateway(172, 20, 10, 1);
//IPAddress subnet(255, 255, 255, 0);
IPAddress subnet(255, 255, 255, 240);
Comms::Packet packet = {.id = 101, .len = 0};

// uint8_t LED_0 = 18;
// uint8_t LED_1 = 19;
// uint8_t LED_2 = 20;
// uint8_t LED_3 = 21;
// uint8_t LED_4 = 7;
// uint8_t LED_5 = 33;
// uint8_t LED_6 = 34;
// uint8_t LED_7 = 35;
// bool led0 = false;

// void initLEDs(){
//   pinMode(26, INPUT); 
//   pinMode(LED_0, OUTPUT);
//   pinMode(LED_1, OUTPUT);
//   pinMode(LED_2, OUTPUT);
//   pinMode(LED_3, OUTPUT);
//   pinMode(LED_4, OUTPUT);
//   pinMode(LED_5, OUTPUT);
//   pinMode(LED_6, OUTPUT);
//   pinMode(LED_7, OUTPUT);
//   digitalWrite(LED_0, LOW);
//   digitalWrite(LED_1, LOW);
//   digitalWrite(LED_2, LOW);
//   digitalWrite(LED_3, LOW);
//   digitalWrite(LED_4, LOW);
//   digitalWrite(LED_5, LOW);
//   digitalWrite(LED_6, LOW);
//   digitalWrite(LED_7, LOW);
// }

uint32_t udpRead(){
    if(WiFi.status() != WL_CONNECTED) {
        Serial.print("..connecting..");
        return 500*1000;
    }
    Serial.print("[Server Connected] ");
 	Serial.println (WiFi.localIP());
    if (udp.available()) {
        //digitalWrite(LED_2, HIGH);
        int packetSize = udp.parsePacket();
        Serial.print(" Received packet from : "); Serial.println(udp.remoteIP());
        Serial.print(" Size : "); Serial.println(packetSize);
        Serial.print(" Data : ");
        if (packetSize) {
 			int len = udp.read(packetBuffer, 255);
 			if (len > 0) packetBuffer[len - 1] = 0;
 			Serial.print(packetBuffer);
 	    }
 	    Serial.println();
    } 
    else {
        //digitalWrite(LED_2, LOW);
    }
    return 500*1000;
}

uint32_t udpSend(){
    //Serial.println(sizeof(Comms::Packet));
    udp.beginPacket(groundStation1, localPort);
    //digitalWrite(LED_3, HIGH);

    packet.len = 0;
    Comms::packetAddFloat(&packet, 420.69);
    Comms::finishPacket(&packet);
    //Serial.println(sizeof(packet));

    udp.write(packet.id);
    udp.write(packet.len);
    udp.write(packet.timestamp, 4);
    udp.write(packet.checksum, 2);
    udp.write(packet.data, packet.len);

    udp.endPacket();
    //digitalWrite(LED_3, LOW);

    return 50*1000;
}

// uint32_t leds(){
//     //first led blinks regularly
//     if (!led0){
//         digitalWrite(LED_0, HIGH);
//         led0 = true;
//     }
//     else{
//         digitalWrite(LED_0, LOW);
//         led0 = false;
//     }
//     //second led on when connected to wifi
//     if (WiFi.status() == WL_CONNECTED){
//         digitalWrite(LED_1, HIGH);
//     }
//     else{
//         digitalWrite(LED_1, LOW);
//     }
//     return 500*1000;
    
// }

Task taskTable[] = {
  //{task_example, 0, true},
    {udpRead, 0, true},
    {udpSend, 0, true},
    //{leds, 0, true},

};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

void setup() {
  // setup stuff here
    Serial.begin(921600);
    //initLEDs();
    char ssid[] = NET_SSID;
    char password[] = NET_PASSWORD;
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println(WiFi.macAddress());
    WiFi.setAutoReconnect(true);
    udp.begin(localPort);

    while(1) {
        // main loop here to avoid arduino overhead
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
        uint32_t ticks = micros(); // current time in microseconds
        if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
            taskTable[i].nexttime = ticks + taskTable[i].taskCall();
        }
    }
    //Comms::processWaitingPackets();
  }
}

void loop() {} // unused