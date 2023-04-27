

#include "FlightStatus.h"

#define STATE_SIZE 3
#define OBS_SIZE 3
#define DELAY_TIME 1

float GROUND_PRESSURE_HPA = 1013;
float baro_altitude = 0;
float gps_altitude = 0;
float acceleration = 0;

float accel_x = 0;
float accel_z = 0;

float accel_x_2 = 0;
float accel_y_2 = 0;
float accel_z_2 = 0;

float longitude = 0;
float latitude = 0;

outputData kxData;

float baro_pressure_2 = 0;
float baro_temp_2 = 0;

Adafruit_BMP3XX bmp;

Adafruit_LSM6DSO32 dso32;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

SFE_UBLOX_GNSS neom9n;

MS5xxx ms5607(&Wire);

SparkFun_KX134 kx134;

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

namespace FlightStatus 
{ 
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

  void set_barometer_ground_altitude() {
    float output = 0;
    for (int i=0; i < 100; i++) {
        output += (bmp.readPressure() / 100.0);
    }
    output = output / 100;
    GROUND_PRESSURE_HPA = output;
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

  float get_gps_altitude() {
    // default is in mm
    return neom9n.getAltitude() / 1000;
  }

  float get_latitude() { 
    return neom9n.getLatitude() / 1e7;
  }

  float get_longitude() { 
    return neom9n.getLongitude() / 1e7;
  }

  uint8_t get_gps_sats() { 
    return (uint8_t)(neom9n.getSIV());
  }

  // float get_baro2_temp() { 
  //   return ms5607.GetTemp();
  // }

  void read_baro2() { 
    ms5607.ReadProm();
    ms5607.Readout();
    baro_pressure_2 = ms5607.GetPres();
    baro_temp_2 = ms5607.GetTemp();
  }

  void update_accel_2() { 
    if (kx134.dataReady())
    {
      kx134.getAccelData(&kxData);
    }
    accel_x_2 = kxData.xData;
    accel_y_2 = kxData.yData;
    accel_z_2 = kxData.zData;
  }
  
  unsigned long lastExecutedMillis = 0; // create a variable to save the last executed time

  uint32_t updateRate = 100 * 1000;

  KalmanFilter filter1(Config::transition, Config::transition_dt, Config::state_noise, Config::obs, Config::obs_noise);

  void init(){

    if (neom9n.begin() == false) //Connect to the u-blox module using Wire port
    {
      Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
      while (1);
    }

    neom9n.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    neom9n.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
    neom9n.setNavigationFrequency(40);
    neom9n.setAutoPVT(true);

    if (!bmp.begin_I2C(0x77)) {
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

    if (!kx134.begin()) {
      Serial.println("Could not communicate with the the KX13X. Freezing.");
      while (1);
    }

    // if (kx134.softwareReset())
    //   Serial.println("Reset.");

    // Give some time for the accelerometer to reset.
    // It needs two, but give it five for good measure.
    delay(2);
    kx134.enableAccel(false);
    kx134.setRange(SFE_KX134_RANGE64G);         // 16g for the KX134
    kx134.enableDataEngine(); // Enables the bit that indicates data is ready.
    // kx134.setOutputDataRate(); // Default is 50Hz
    kx134.enableAccel();

    if(ms5607.connect()>0) {
      Serial.println("Error connecting...");
    }
  }

  uint8_t launched = 0;
  uint8_t burnout = 0;
  uint8_t apogee = 0;
  uint8_t main_parachute = 0;
  float prev_altitude = 0;
  uint8_t deploy_vel = 0;
  uint16_t main_altitude = 500;
  uint8_t would_deploy_drogue = 0;
  uint8_t would_deploy_main = 0;
  

  // Packet definitions
  Comms::Packet filteredPacket = {.id = 2};
  Comms::Packet imuPacket = {.id = 3};
  Comms::Packet gpsPacket = {.id = 4};
  Comms::Packet baroPacket = {.id = 5};
  Comms::Packet eventsPacket = {.id = 10};

  uint32_t updateFlight() { 
    baro_altitude = get_barometer_altitude();
    acceleration = get_acceleration_y();
    gps_altitude = get_gps_altitude();

    update_accel_2();

    // read_baro2();

    longitude = get_longitude();
    latitude = get_latitude();

    accel_x = get_acceleration_x();
    accel_z = get_acceleration_z();

    float gyro_x = get_gyro_x();
    float gyro_y = get_gyro_y();
    float gyro_z = get_gyro_z();

    //   acceleration = acceleration / 9.81; // convert to g

      if (gps_altitude < 50) {
       gps_altitude = baro_altitude;
      }

      // if (0.8 < acceleration/9.81 & 1.2 > acceleration/9.81) {
      //  acceleration = 0;
      // }

      // if (acceleration < 0) {
      //  acceleration = 0;
      // }
    
    filter1.update(gps_altitude, baro_altitude, acceleration);
    float alt = filter1.get_altitude();
    float vel = filter1.get_velocity();
    float accl = filter1.get_acceleration();

    Serial.print("baroAltitude:"); Serial.print(baro_altitude); Serial.print(" ");
    Serial.print("GPSAltitude:"); Serial.print(gps_altitude); Serial.print("  ");
    Serial.print("Acceleration:"); Serial.print(acceleration); Serial.println("  ");
    Serial.print("FilteredVelocity:"); Serial.print(vel); Serial.print("  ");
    Serial.print("FilteredAcceleration:"); Serial.print(accl); Serial.print("  ");
    Serial.print("FilteredAltitude:"); Serial.print(alt); Serial.println();  

    // for (int i = 0; i < 10; i++) { 
      imuPacket.len = 0;
      Comms::packetAddFloat(&imuPacket, accel_x);
      Comms::packetAddFloat(&imuPacket, acceleration);
      Comms::packetAddFloat(&imuPacket, accel_z);
      Comms::packetAddFloat(&imuPacket, gyro_x);
      Comms::packetAddFloat(&imuPacket, gyro_y);
      Comms::packetAddFloat(&imuPacket, gyro_z);
      Comms::packetAddFloat(&imuPacket, accel_x_2);
      Comms::packetAddFloat(&imuPacket, accel_y_2);
      Comms::packetAddFloat(&imuPacket, accel_z_2);

      // emit the packet
      Comms::emitPacketToGS(&imuPacket);
      Radio::forwardPacket(&imuPacket);

      gpsPacket.len = 0;
      Comms::packetAddFloat(&gpsPacket, gps_altitude);
      Comms::packetAddFloat(&gpsPacket, latitude);
      Comms::packetAddFloat(&gpsPacket, longitude);
      Comms::packetAddUint8(&gpsPacket, get_gps_sats());

      // emit the packets
      Comms::emitPacketToGS(&gpsPacket);
      Radio::forwardPacket(&gpsPacket);

      baroPacket.len = 0;
      Comms::packetAddFloat(&baroPacket, baro_altitude);
      Comms::packetAddFloat(&baroPacket, baro_pressure_2);
      Comms::packetAddFloat(&baroPacket, baro_temp_2);

      // emit the packets
      Comms::emitPacketToGS(&baroPacket);
      Radio::forwardPacket(&baroPacket);

      filteredPacket.len = 0;
      Comms::packetAddFloat(&filteredPacket, alt);
      Comms::packetAddFloat(&filteredPacket, vel);
      Comms::packetAddFloat(&filteredPacket, accl);

      // emit the packets
      Comms::emitPacketToGS(&filteredPacket);
      Radio::forwardPacket(&filteredPacket);
    // }

    if (vel > 3 & accl > 15 & launched == 0) {
      launched = 100;
    }

    if (launched > 0 & accl < 5 & burnout == 0) {
      burnout = 100;
    }

    if (burnout > 0 & alt < prev_altitude & apogee == 0 & vel < deploy_vel) {
      apogee = 100;
      // deploy_drogue();
      would_deploy_drogue = 42;
    }

    if (apogee > 0 & alt < main_altitude & main_parachute == 0) {
      main_parachute = alt;
      // deploy_main();
      would_deploy_main = 42;
    } 

    eventsPacket.len = 0;
    Comms::packetAddUint8(&eventsPacket, launched);
    Comms::packetAddUint8(&eventsPacket, burnout);
    Comms::packetAddUint8(&eventsPacket, apogee);
    Comms::packetAddUint8(&eventsPacket, would_deploy_drogue);
    Comms::packetAddUint8(&eventsPacket, would_deploy_main);
    Comms::emitPacketToGS(&eventsPacket);
    Radio::forwardPacket(&eventsPacket);
    return updateRate;
  }

}
