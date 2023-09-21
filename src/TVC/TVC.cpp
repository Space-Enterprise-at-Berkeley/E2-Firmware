#include "TVC.h"
#include "EEPROM.h"
#include "HAL.h"

namespace TVC {

    // define update period
    uint32_t tvcUpdatePeriod = 20 * 1000;
    int x_motor_ticksp = 0;
    int y_motor_ticksp = 0;
    float x_p = 0.6;
    float x_i = 0.01;
    float x_d = 0;
    float y_p = 0.6;
    float y_i = 0.01;
    float y_d = 0;
    int MID_SPD = 307;
    //(4096 * (1500 / 200000)))
    int MAX_SPD = 25;
    int MIN_SPD = -25;
    int INNER_BUFFER_SIZE = 2;
    
    int speed_x = 0;
    int speed_y = 0;

    int idx = 0;
    int idy = 150;

    // just for records
    int max_x_ticks = 500;
    int min_x_ticks = -500;

    int tvcState = 0; //0: constant power move, 1: pid control
    int circleEnabled = false;

    int circleTicks[] = {
        656,
        663,
        671,
        679,
        687,
        695,
        702,
        710,
        718,
        726,
        734,
        742,
        750,
        757,
        765,
        773,
        781,
        789,
        797,
        804,
        812,
        820,
        828,
        836,
        843,
        851,
        859,
        867,
        875,
        882,
        890,
        898,
        905,
        913,
        921,
        928,
        936,
        944,
        951,
        959,
        966,
        974,
        981,
        989,
        996,
        1004,
        1011,
        1018,
        1026,
        1033,
        1040,
        1047,
        1055,
        1062,
        1069,
        1076,
        1083,
        1090,
        1097,
        1104,
        1111,
        1118,
        1125,
        1131,
        1138,
        1145,
        1151,
        1158,
        1165,
        1171,
        1178,
        1184,
        1190,
        1197,
        1203,
        1209,
        1215,
        1221,
        1228,
        1234,
        1240,
        1245,
        1251,
        1257,
        1263,
        1269,
        1274,
        1280,
        1285,
        1291,
        1296,
        1302,
        1307,
        1312,
        1317,
        1322,
        1327,
        1332,
        1337,
        1342,
        1347,
        1352,
        1356,
        1361,
        1365,
        1370,
        1374,
        1379,
        1383,
        1387,
        1391,
        1395,
        1399,
        1403,
        1407,
        1410,
        1414,
        1418,
        1421,
        1425,
        1428,
        1431,
        1435,
        1438,
        1441,
        1444,
        1447,
        1450,
        1452,
        1455,
        1458,
        1460,
        1463,
        1465,
        1467,
        1470,
        1472,
        1474,
        1476,
        1478,
        1480,
        1481,
        1483,
        1485,
        1486,
        1488,
        1489,
        1490,
        1491,
        1493,
        1494,
        1495,
        1495,
        1496,
        1497,
        1497,
        1498,
        1498,
        1499,
        1499,
        1499,
        1499,
        1500,
        1499,
        1499,
        1499,
        1499,
        1498,
        1498,
        1497,
        1497,
        1496,
        1495,
        1495,
        1494,
        1493,
        1491,
        1490,
        1489,
        1488,
        1486,
        1485,
        1483,
        1481,
        1480,
        1478,
        1476,
        1474,
        1472,
        1470,
        1467,
        1465,
        1463,
        1460,
        1458,
        1455,
        1452,
        1450,
        1447,
        1444,
        1441,
        1438,
        1435,
        1431,
        1428,
        1425,
        1421,
        1418,
        1414,
        1410,
        1407,
        1403,
        1399,
        1395,
        1391,
        1387,
        1383,
        1379,
        1374,
        1370,
        1365,
        1361,
        1356,
        1352,
        1347,
        1342,
        1337,
        1332,
        1327,
        1322,
        1317,
        1312,
        1307,
        1302,
        1296,
        1291,
        1285,
        1280,
        1274,
        1269,
        1263,
        1257,
        1251,
        1245,
        1240,
        1234,
        1228,
        1221,
        1215,
        1209,
        1203,
        1197,
        1190,
        1184,
        1178,
        1171,
        1165,
        1158,
        1151,
        1145,
        1138,
        1131,
        1125,
        1118,
        1111,
        1104,
        1097,
        1090,
        1083,
        1076,
        1069,
        1062,
        1055,
        1047,
        1040,
        1033,
        1026,
        1018,
        1011,
        1004,
        996,
        989,
        981,
        974,
        966,
        959,
        951,
        944,
        936,
        928,
        921,
        913,
        905,
        898,
        890,
        882,
        875,
        867,
        859,
        851,
        843,
        836,
        828,
        820,
        812,
        804,
        797,
        789,
        781,
        773,
        765,
        757,
        750,
        742,
        734,
        726,
        718,
        710,
        702,
        695,
        687,
        679,
        671,
        663,
        656,
        648,
        640,
        632,
        624,
        617,
        609,
        601,
        594,
        586,
        578,
        571,
        563,
        555,
        548,
        540,
        533,
        525,
        518,
        510,
        503,
        495,
        488,
        481,
        473,
        466,
        459,
        452,
        444,
        437,
        430,
        423,
        416,
        409,
        402,
        395,
        388,
        381,
        375,
        368,
        361,
        354,
        348,
        341,
        334,
        328,
        321,
        315,
        309,
        302,
        296,
        290,
        284,
        278,
        271,
        265,
        259,
        254,
        248,
        242,
        236,
        230,
        225,
        219,
        214,
        208,
        203,
        197,
        192,
        187,
        182,
        177,
        172,
        167,
        162,
        157,
        152,
        147,
        143,
        138,
        134,
        129,
        125,
        120,
        116,
        112,
        108,
        104,
        100,
        96,
        92,
        89,
        85,
        81,
        78,
        74,
        71,
        68,
        64,
        61,
        58,
        55,
        52,
        49,
        47,
        44,
        41,
        39,
        36,
        34,
        32,
        29,
        27,
        25,
        23,
        21,
        19,
        18,
        16,
        14,
        13,
        11,
        10,
        9,
        8,
        6,
        5,
        4,
        4,
        3,
        2,
        2,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        2,
        2,
        3,
        4,
        4,
        5,
        6,
        8,
        9,
        10,
        11,
        13,
        14,
        16,
        18,
        19,
        21,
        23,
        25,
        27,
        29,
        32,
        34,
        36,
        39,
        41,
        44,
        47,
        49,
        52,
        55,
        58,
        61,
        64,
        68,
        71,
        74,
        78,
        81,
        85,
        89,
        92,
        96,
        100,
        104,
        108,
        112,
        116,
        120,
        125,
        129,
        134,
        138,
        143,
        147,
        152,
        157,
        162,
        167,
        172,
        177,
        182,
        187,
        192,
        197,
        203,
        208,
        214,
        219,
        225,
        230,
        236,
        242,
        248,
        254,
        259,
        265,
        271,
        278,
        284,
        290,
        296,
        302,
        309,
        315,
        321,
        328,
        334,
        341,
        348,
        354,
        361,
        368,
        375,
        381,
        388,
        395,
        402,
        409,
        416,
        423,
        430,
        437,
        444,
        452,
        459,
        466,
        473,
        481,
        488,
        495,
        503,
        510,
        518,
        525,
        533,
        540,
        548,
        555,
        563,
        571,
        578,
        586,
        594,
        601,
        609,
        617,
        624,
        632,
        640,
        648
    };


    // define class variables    
    PIDController x_Controller(
        x_p, x_i, x_d, 
        MIN_SPD, MAX_SPD, 
        PIDController::standard, 
        INNER_BUFFER_SIZE);

    PIDController y_Controller(
        y_p, y_i, y_d, 
        MIN_SPD, MAX_SPD, 
        PIDController::standard, 
        INNER_BUFFER_SIZE);

    void init() {
        x_Controller.reset();
        y_Controller.reset();
        delay(3000); 
    }


    void definePosition(Comms::Packet defPos, uint8_t id) {
        // Serial.println("Packet");
        float x_pos = packetGetFloat(&defPos, 0);
        float y_pos = packetGetFloat(&defPos, 4);

        x_motor_ticksp = (int)x_pos;
        y_motor_ticksp = (int)y_pos;

        Serial.println("Received command to define encoder position: (" + String(x_pos) + ", " + String(y_pos) + ")");
        Serial.println("X setpoint is: " + String(x_motor_ticksp) + ", Y setpoint is: " + String(y_motor_ticksp));
    }

    void defineSpeeds(Comms::Packet defSpeeds, uint8_t id) {
        setXSpeed(packetGetUint8(&defSpeeds, 0));
        setYSpeed(packetGetUint8(&defSpeeds, 4));
    }

    uint32_t moveTVC() {

        if (tvcState) { // moving with PID is state 1
            // Serial.println("PID time");

            if (circleEnabled) { 
                
                x_motor_ticksp = 2*circleTicks[idx]/3 - 500;
                y_motor_ticksp = 2*circleTicks[idy]/3 - 500;

                idx += 2;
                idy += 2;
                if(idx >= sizeof(circleTicks)/4) idx = 0;
                if(idy >= sizeof(circleTicks)/4) idy = 0;
            }

            speed_x = x_Controller.update(x_motor_ticksp - HAL::getEncoderCount_0());

            speed_y = y_Controller.update(y_motor_ticksp - HAL::getEncoderCount_1());

            // Serial.println("X encoder count: " + String(HAL::getEncoderCount_0()));
            // Serial.println("Y encoder count: " + String(HAL::getEncoderCount_1()));
        } 

            Comms::Packet tmp = {.id=42};
            Comms::packetAddUint32(&tmp, HAL::getEncoderCount_0());
            Comms::packetAddUint32(&tmp, HAL::getEncoderCount_1());
            Comms::emitPacketToGS(&tmp);

            Comms::Packet stp = {.id=43};
            Comms::packetAddUint32(&stp, x_motor_ticksp);
            Comms::packetAddUint32(&stp, y_motor_ticksp);
            Comms::emitPacketToGS(&stp);

            if ((millis() % 1000)  >= 0) {
                Serial.printf("encX @ %d, encY @ %d\n", HAL::getEncoderCount_0(), HAL::getEncoderCount_1());
                Serial.printf("speedX @ %d, speedY @ %d\n", speed_x + MID_SPD, speed_y + MID_SPD);
            }

            ledcWrite(0, speed_x + MID_SPD);
            ledcWrite(1, speed_y + MID_SPD);
            
            return tvcUpdatePeriod;
    }

    void setMode(int mode) { 
        tvcState = mode;
        // mode 0: manual, 1: PID
    }

    void setTVCMode(Comms::Packet packet, uint8_t ip) { 
        tvcState = (int)(packetGetUint8(&packet, 0));
    }

    void enableCircle(Comms::Packet packet, uint8_t ip) { 
        setMode(1);
        circleEnabled = 1;// (int)(packetGetUint8(&packet, 0));
        Serial.println("Circle enabled");
    }

    void setXSpeed(int spdx) { 
        speed_x = spdx;
    }

    void setYSpeed(int spdy) { 
        speed_y = spdy;
    }

    void stopTVC(Comms::Packet packet, uint8_t ip) { 
        setMode(0);
        circleEnabled = 0;
        speed_x = 0;
        speed_y = 0;
    }

    uint32_t zero() {
        static int zeroState = 0;
        circleEnabled = 0;
        switch(zeroState) { 
            case 0:
                setMode(0);
                setXSpeed(-3);
                setYSpeed(-3);
                ledcWrite(0, MID_SPD + speed_x);
                ledcWrite(1, MID_SPD + speed_y);
                zeroState = 1;
                Serial.println("in zeroing");
                return 10*1000*1000;
            case 1:
                HAL::setEncoderCount_0(650);
                HAL::setEncoderCount_1(650);

                x_motor_ticksp = 0;
                y_motor_ticksp = 0;
                setMode(1);
                Serial.println("done zeroing");
                return 0; //disables
        }
        return 0;
    }
    
}
