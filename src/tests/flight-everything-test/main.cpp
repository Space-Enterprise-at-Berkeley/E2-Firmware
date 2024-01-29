#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SCK  18
#define MISO  21
#define MOSI  17
#define CS  14

SPIClass spi = SPIClass(HSPI);


float baro_altitude = 0;
float gps_altitude = 0;
float acceleration = 0;

float a_x = 0;
float a_z = 0;



static const int RXPin = 34, TXPin = 33;
static const uint32_t GPSBaud = 4800;



#define STATE_SIZE 3
#define OBS_SIZE 3

#include <ArduinoEigenDense.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_LSM6DSO32.h>
// #include <TinyGPSPlus.h>


float GROUND_PRESSURE_HPA = 1013;
Adafruit_BMP3XX bmp;

Adafruit_LSM6DSO32 dso32;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

// TinyGPSPlus gps;

using namespace Eigen;

namespace Config{
    const Matrix<double, STATE_SIZE, STATE_SIZE> transition { 
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    const Matrix<double, STATE_SIZE, STATE_SIZE> transition_dt { 
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 0}
    };
    const Matrix<double, STATE_SIZE, STATE_SIZE> state_noise {
        {0.001, 0, 0},
        {0, 0.9, 0},
        {0, 0, 0.9},
    };
    const Matrix<double, OBS_SIZE, STATE_SIZE> obs {
        {1, 0, 0},
        {1, 0, 0},
        {0 ,0 ,1/9.8}
    };
    const Matrix<double, OBS_SIZE, OBS_SIZE> obs_noise {
        {10, 0 ,0},
        {0, 10000, 0},
        {0, 0, 0.01}
    };
}









using namespace Eigen;

class KalmanFilter{
    /*
    Kalman filter for apogee detection
    Observation and state space dimensions are fixed through configs and defs
    Dynamics, measurement, and noise matrix values can be modified per filter instance
    Variables:
    A = Transition matrix (STATE_SIZE x STATE_SIZE)
    Q = State noise covariance (STATE_SIZE x STATE_SIZE)
    C = Measurement matrix (OBS_SIZE x STATE_SIZE)
    R = Measurement noise covariance (OBS_SIZE x OBS_SIZE)
    */

    private:
    Matrix<double, STATE_SIZE, STATE_SIZE> A;
    Matrix<double, STATE_SIZE, STATE_SIZE> A_dt;
    Matrix<double, STATE_SIZE, STATE_SIZE> Q;
    Matrix<double, OBS_SIZE, STATE_SIZE> C;
    Matrix<double, OBS_SIZE, OBS_SIZE> R;

    Vector<double, STATE_SIZE> state;
    Matrix<double, STATE_SIZE, STATE_SIZE> P;
    Matrix<double, STATE_SIZE, STATE_SIZE> prev_P;

    Vector<double, OBS_SIZE> measurement;
    unsigned long last_update_micros;

    const Matrix<double, STATE_SIZE, STATE_SIZE> I = Matrix<double, STATE_SIZE, STATE_SIZE>::Identity();

    void predict(double dt);
    void _update(Vector<double, OBS_SIZE>& measurement, double dt);
    Vector<double, STATE_SIZE>& state_estimate();

    public:
    KalmanFilter(
        const Matrix<double, STATE_SIZE, STATE_SIZE>& transition,
        const Matrix<double, STATE_SIZE, STATE_SIZE>& transition_dt,
        const Matrix<double, STATE_SIZE, STATE_SIZE>& state_noise,
        const Matrix<double, OBS_SIZE, STATE_SIZE>& obs,
        const Matrix<double, OBS_SIZE, OBS_SIZE>& obs_noise
    ); 

    void reset();
    void update(double gps, double barometer, double accel);
    double get_altitude();
    double get_velocity();
    double get_acceleration();
};











KalmanFilter::KalmanFilter(
    const Matrix<double, STATE_SIZE, STATE_SIZE>& transition,
    const Matrix<double, STATE_SIZE, STATE_SIZE>& transition_dt,
    const Matrix<double, STATE_SIZE, STATE_SIZE>& state_noise,
    const Matrix<double, OBS_SIZE, STATE_SIZE>& obs,
    const Matrix<double, OBS_SIZE, OBS_SIZE>& obs_noise
) {
    A = transition;
    A_dt = transition_dt;
    Q = state_noise;
    C = obs;
    R = obs_noise;

    reset();
}

void KalmanFilter::reset() {
    state = Vector<double, STATE_SIZE>::Zero();
    prev_P = Matrix<double, STATE_SIZE, STATE_SIZE>::Zero();
    P = Matrix<double, STATE_SIZE, STATE_SIZE>::Zero();
    measurement = Vector<double, OBS_SIZE>::Zero();
}

void KalmanFilter::predict(double dt) {
    prev_P = P; // I think this deepcopies P, but not rly sure tbh ¯\_(ツ)_/¯
    Matrix<double, STATE_SIZE, STATE_SIZE> A_current = (A + A_dt * dt);
    state = A_current * state;
    P = A_current * P * A_current.transpose() + Q * dt;
}

void KalmanFilter::_update(Vector<double, OBS_SIZE>& measurement, double dt) {
    predict(dt);
    Matrix<double, STATE_SIZE, STATE_SIZE> K = P * C.transpose() * (C * P * C.transpose() + R).inverse();
    P = (I - K * C) * P;
    state += K * (measurement - C * state);
}

Vector<double, STATE_SIZE>& KalmanFilter::state_estimate() {
    return state;
}

void KalmanFilter::update(double gps, double barometer, double accel) {
    /*
    Updates Kalman filter with latest measurement. This function should only be called when we have new updates for ALL 3 sensors
    @param gps: gps height in m
    @param barometer: barometer altitude in m
    @param accel: accelerometer reading (in vertical direction) in g
    */


    measurement(0) = gps;
    measurement(1) = barometer;
    measurement(2) = accel;
   
    unsigned long t1 = micros();
    double dt = (t1 - last_update_micros)/1e6;
    last_update_micros = t1;
    _update(measurement, dt);
    
}

double KalmanFilter::get_altitude() {
    /*
    Returns current state estimate of altitude, as of previous update
    @return altitude in m
    */
    return state(0);
}
double KalmanFilter::get_velocity() {
    /*
    Returns current state estimate of vertical velocity, as of previous update
    @return velocity in m/s
    */
    return state(1);
}
double KalmanFilter::get_acceleration() {
    /*
    Returns current state estimate of vertical acceleration, as of previous update
    @return acceleration in m/s^2
    */
    return state(2);
}

void init_gps() {
   Serial1.write(0x10);
   Serial1.write(0x7A);
   Serial1.write((uint8_t)0x00);
   Serial1.write(0x01);
   Serial1.write((uint8_t)0x00);
   Serial1.write((uint8_t)0x00);
   Serial1.write((uint8_t)0x00);
   Serial1.write(0b00111111);
   Serial1.write(0x10);
   Serial1.write(0x03);

   delay(200);
   Serial1.write(0x10);
   Serial1.write(0x69);
   Serial1.write(0x01);
   Serial1.write((uint8_t)0x00);
   Serial1.write(0x10);
   Serial1.write(0x03);
}

void set_barometer_ground_altitude() {
  float output = 0;
  for (int i=0; i < 100; i++) {
      output += (bmp.readPressure() / 100.0);
  }
  output = output / 100;
  GROUND_PRESSURE_HPA = output;
}



void updateSensors(String rowdata) {
  // baro_altitude = getValue(rowdata, ',', 0).toFloat();
  // acceleration = getValue(rowdata, ',', 1).toFloat();
//   gps_altitude = getValue(rowdata, ',', 2).toFloat();
}




float get_barometer_altitude() {
  float output;
  output = bmp.readAltitude(GROUND_PRESSURE_HPA);
  return output;
}


float get_barometer_temperature() {
  float output;
  output = bmp.temperature;
  return output;
}

float get_barometer_pressure() {
  float output;
  output = bmp.pressure / 100.0;
  return output;
}

float get_acceleration_x() {
  float output;
  dso32.getEvent(&accel, &gyro, &temp);
  output = accel.acceleration.x;
  return output;
}

float get_acceleration_y() {
  float output;
  dso32.getEvent(&accel, &gyro, &temp);
  output = accel.acceleration.y;
  return output;
}

float get_acceleration_z() {
  float output;
  dso32.getEvent(&accel, &gyro, &temp);
  output = accel.acceleration.z;
  return output;
}

float get_gyro_x() {
  float output;
  dso32.getEvent(&accel, &gyro, &temp);
  output = gyro.gyro.x;
  return output;
}

float get_gyro_y() {
  float output;
  dso32.getEvent(&accel, &gyro, &temp);
  output = gyro.gyro.y;
  return output;
}

float get_gyro_z() {
  float output;
  dso32.getEvent(&accel, &gyro, &temp);
  output = gyro.gyro.z;
  return output;
}

// float get_gps_altitude() {
//   return (float)gps.altitude.meters();
// }




// String getValue(String data, char separator, int index)
// {
//     int found = 0;
//     int strIndex[] = { 0, -1 };
//     int maxIndex = data.length() - 1;

//     for (int i = 0; i <= maxIndex && found <= index; i++) {
//         if (data.charAt(i) == separator || i == maxIndex) {
//             found++;
//             strIndex[0] = strIndex[1] + 1;
//             strIndex[1] = (i == maxIndex) ? i+1 : i;
//         }
//     }
//     return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
// }

// String printLineN(fs::FS &fs, float lineNumber, const char * path){
//   String output = "";
//   char cr;

//   File file = fs.open(path);
//   if(!file){
//     Serial.println("Failed to open file for reading");
//     return output;
//   }
  
//   for(unsigned int i = 0; i < (lineNumber - 1);){
//     cr = file.read();
//     if(cr == 10){
//       i++;
//     }
//   }
  
  
//   while (file.available()) {
//     char newChar = file.read();
//     if (newChar == '\n') {
//       return output;
//     }
//     output += newChar;
//   }
//   file.close();
// }



#define DELAY_TIME 1
 
unsigned long lastExecutedMillis = 0; // create a variable to save the last executed time



KalmanFilter filter1(Config::transition, Config::transition_dt, Config::state_noise, Config::obs, Config::obs_noise);

void setup(){
  // Serial.begin(115200);
//   Serial1.begin(GPSBaud, SERIAL_8N1, TXPin, RXPin);
//   spi.begin(SCK, MISO, MOSI, CS);
//   init_gps();
  // Wire.begin(1,2);

//   if (!SD.begin(CS,spi,80000000)) {
//     Serial.println("Card Mount Failed");
//     return;
//   }    

  Serial.begin(921600);
  Serial.println("Initializing....");
  Wire.setClock(400000);
  Wire.setPins(1,2);
  Wire.begin();

  
  if (!bmp.begin_I2C(0x76)) {
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  if (!dso32.begin_I2C()) {
    while (1) {
      delay(10);
    }
  }

  dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_16_G);
  dso32.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS );
  dso32.setAccelDataRate(LSM6DS_RATE_3_33K_HZ);
  dso32.setGyroDataRate(LSM6DS_RATE_3_33K_HZ);


  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_200_HZ);
  bmp.performReading();
  
  set_barometer_ground_altitude();
  
  
}

int i = 0;

void loop(){
  baro_altitude = get_barometer_altitude();
  acceleration = get_acceleration_y();
  a_x = get_acceleration_x();
  a_z = get_acceleration_z();
  
//   gps_altitude = get_gps_altitude();

//  acceleration = acceleration / 9.81; // convert to g
//
//  if (gps_altitude < 50) {
//    gps_altitude = baro_altitude;
//  }
//
//  if (0.8 < acceleration & 1.2 > acceleration) {
//    acceleration = 0;
//  }
//  if (acceleration < 0) {
//    acceleration = 0;
//  }
//  
//  filter1.update(gps_altitude, baro_altitude, acceleration);
//  
//  double alt = filter1.get_altitude();
//  double vel = filter1.get_velocity();
//  double accl = filter1.get_acceleration();
//  
////  acceleration = acceleration * 50;
////  accl = accl * 10;
////  vel = vel * 5;
  
  Serial.print("baroAltitude:"); Serial.print(baro_altitude); Serial.print(" ");
//   Serial.print("GPSAltitude:"); Serial.print(gps_altitude); Serial.print("  ");
  Serial.print("Accel x:"); Serial.print(a_x); Serial.print(" ");
  Serial.print("Accel y:"); Serial.print(acceleration); Serial.print("  ");
  Serial.print("Accel z:"); Serial.print(a_z); Serial.println("  ");
//  Serial.print("FilteredVelocity:"); Serial.print(vel); Serial.print("  ");
//  Serial.print("FilteredAcceleration:"); Serial.print(accl); Serial.print("  ");
//  Serial.print("FilteredAltitude:"); Serial.print(alt); Serial.println();
    
}