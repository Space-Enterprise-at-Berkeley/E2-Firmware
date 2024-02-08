#include <Arduino.h>
#include <MCP23008.h>

// sel0 through sel2 are the MUX select pins, currpins and contpin are th pins via which readings can be taken via analog read
uint8_t sel0, sel1, sel2, currpin, contpin;

// update period for the current/continuity task
uint32_t cmUpdatePeriod;

// publically accessible via getters, last updated values held here
float currents[8] = {};
float continuities[8] = {};

// voltage threshold for continuity to be detected
float CONT_THRESHOLD = 2.5;

// Minimum current draw to determine if a motor is running or not
float RUNNING_THRESH = 0.1;


// sets up mux's and IO expanders
void init(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t curr, uint8_t cont){
    sel0 = s0;
    sel1 = s1;
    sel2 = s2;
    currpin = curr;
    contpin = cont;

    // every 10 ms
    cmUpdatePeriod = 1000 * 100;

    pinMode(sel0, OUTPUT);
    pinMode(sel1, OUTPUT);
    pinMode(sel2, OUTPUT);
    pinMode(currpin, INPUT);
    pinMode(contpin, INPUT);
}

// converts ADC current counts to current in amps
// Current is read as follows: the actual current / 5000 is sourced across a 4.53k resistor yielding a read voltage
// Read out of 4096 ADC counts where 4096 = 3.3V
// TODO characterise the current to ADC counts relationship better (via emperical testing with the E-Load)
float adcToCurrent(uint16_t counts) {
    return (counts / 4096.0) * 4530;
}


// reads currents and continuity, reports them via packets and by setting the above arrays
// also updates relevant LEDs based on thresholds
void readChannels() {
    // iterate through MUX channels
    for (int i = 0; i < 8; i ++){
        digitalWrite(sel0, i & 0x01);
        digitalWrite(sel1, (i >> 1) & 0x01);
        digitalWrite(sel2, (i >> 2) & 0x01);

        // read raw current and continuity voltages in ADC counts
        uint16_t rawCont = analogRead(contpin);
        uint16_t rawCurr = analogRead(currpin);

        // convert counts to voltages / currents
        float cont = (rawCont / 4096.0) * 3.3;
        float curr = adcToCurrent(rawCurr);

        continuities[i] = cont;
        currents[i] = curr;
    } 
}

// getters
float* getCurrents() {
    return currents;
}

float* getContinuities() {
    return continuities;
}

bool isChannelContinuous(uint8_t channel) {
    return continuities[channel] > CONT_THRESHOLD;
}

void setup() { 
    Serial.begin(921600);
    Serial.println("Initializing....");
    init(40, 39, 38, 15, 14);
}

void loop() { 
    readChannels();
    Serial.print("Currents:     ");
    for (int i = 0; i < 8; i ++){
        Serial.print(currents[i]);
        Serial.print("...");
    }
    Serial.println();
    Serial.print("Continuity:     ");
    for (int i = 0; i < 8; i ++){
        Serial.print(continuities[i]);
        Serial.print("...");
    }
    Serial.println();
}