#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace bno055 {

// BNO055 Register Map
static const uint8_t BNO055_CHIP_ID_ADDR = 0x00;
static const uint8_t BNO055_PAGE_ID_ADDR = 0x07;
static const uint8_t BNO055_OPR_MODE_ADDR = 0x3D;
static const uint8_t BNO055_PWR_MODE_ADDR = 0x3E;
static const uint8_t BNO055_SYS_TRIGGER_ADDR = 0x3F;
static const uint8_t BNO055_TEMP_ADDR = 0x34;

// Calibration status
static const uint8_t BNO055_CALIB_STAT_ADDR = 0x35;

// Accelerometer data (raw)
static const uint8_t BNO055_ACCEL_DATA_X_LSB_ADDR = 0x08;

// Magnetometer data
static const uint8_t BNO055_MAG_DATA_X_LSB_ADDR = 0x0E;

// Gyroscope data
static const uint8_t BNO055_GYRO_DATA_X_LSB_ADDR = 0x14;

// Euler angles (sensor fusion output)
static const uint8_t BNO055_EULER_H_LSB_ADDR = 0x1A;

// Quaternion data (sensor fusion output)
static const uint8_t BNO055_QUATERNION_DATA_W_LSB_ADDR = 0x20;

// Linear acceleration (sensor fusion: acceleration without gravity)
static const uint8_t BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR = 0x28;

// Gravity vector (sensor fusion: gravity component)
static const uint8_t BNO055_GRAVITY_DATA_X_LSB_ADDR = 0x2E;

// Operation modes
enum BNO055OperationMode {
  OPERATION_MODE_CONFIG = 0x00,
  OPERATION_MODE_NDOF = 0x0C,  // 9DOF sensor fusion
};

// Power modes
enum BNO055PowerMode {
  POWER_MODE_NORMAL = 0x00,
};

class BNO055Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Acceleration sensors
  void set_acceleration_x_sensor(sensor::Sensor *sensor) { acceleration_x_ = sensor; }
  void set_acceleration_y_sensor(sensor::Sensor *sensor) { acceleration_y_ = sensor; }
  void set_acceleration_z_sensor(sensor::Sensor *sensor) { acceleration_z_ = sensor; }

  // Linear acceleration sensors (without gravity)
  void set_linear_acceleration_x_sensor(sensor::Sensor *sensor) { linear_acceleration_x_ = sensor; }
  void set_linear_acceleration_y_sensor(sensor::Sensor *sensor) { linear_acceleration_y_ = sensor; }
  void set_linear_acceleration_z_sensor(sensor::Sensor *sensor) { linear_acceleration_z_ = sensor; }

  // Gravity vector
  void set_gravity_x_sensor(sensor::Sensor *sensor) { gravity_x_ = sensor; }
  void set_gravity_y_sensor(sensor::Sensor *sensor) { gravity_y_ = sensor; }
  void set_gravity_z_sensor(sensor::Sensor *sensor) { gravity_z_ = sensor; }

  // Gyroscope sensors
  void set_gyroscope_x_sensor(sensor::Sensor *sensor) { gyroscope_x_ = sensor; }
  void set_gyroscope_y_sensor(sensor::Sensor *sensor) { gyroscope_y_ = sensor; }
  void set_gyroscope_z_sensor(sensor::Sensor *sensor) { gyroscope_z_ = sensor; }

  // Magnetometer sensors
  void set_magnetometer_x_sensor(sensor::Sensor *sensor) { magnetometer_x_ = sensor; }
  void set_magnetometer_y_sensor(sensor::Sensor *sensor) { magnetometer_y_ = sensor; }
  void set_magnetometer_z_sensor(sensor::Sensor *sensor) { magnetometer_z_ = sensor; }

  // Euler angles (sensor fusion output)
  void set_heading_sensor(sensor::Sensor *sensor) { heading_ = sensor; }
  void set_roll_sensor(sensor::Sensor *sensor) { roll_ = sensor; }
  void set_pitch_sensor(sensor::Sensor *sensor) { pitch_ = sensor; }

  // Quaternion (sensor fusion output)
  void set_quaternion_w_sensor(sensor::Sensor *sensor) { quaternion_w_ = sensor; }
  void set_quaternion_x_sensor(sensor::Sensor *sensor) { quaternion_x_ = sensor; }
  void set_quaternion_y_sensor(sensor::Sensor *sensor) { quaternion_y_ = sensor; }
  void set_quaternion_z_sensor(sensor::Sensor *sensor) { quaternion_z_ = sensor; }

  // Calibration status
  void set_calibration_status_sensor(sensor::Sensor *sensor) { calibration_status_ = sensor; }

  // Temperature
  void set_temperature_sensor(sensor::Sensor *sensor) { temperature_ = sensor; }

 protected:
  bool set_mode_(BNO055OperationMode mode);
  int16_t read_s16_(uint8_t reg);
  bool initialized_ = false;

  // Acceleration sensors
  sensor::Sensor *acceleration_x_{nullptr};
  sensor::Sensor *acceleration_y_{nullptr};
  sensor::Sensor *acceleration_z_{nullptr};

  // Linear acceleration sensors
  sensor::Sensor *linear_acceleration_x_{nullptr};
  sensor::Sensor *linear_acceleration_y_{nullptr};
  sensor::Sensor *linear_acceleration_z_{nullptr};

  // Gravity vector
  sensor::Sensor *gravity_x_{nullptr};
  sensor::Sensor *gravity_y_{nullptr};
  sensor::Sensor *gravity_z_{nullptr};

  // Gyroscope sensors
  sensor::Sensor *gyroscope_x_{nullptr};
  sensor::Sensor *gyroscope_y_{nullptr};
  sensor::Sensor *gyroscope_z_{nullptr};

  // Magnetometer sensors
  sensor::Sensor *magnetometer_x_{nullptr};
  sensor::Sensor *magnetometer_y_{nullptr};
  sensor::Sensor *magnetometer_z_{nullptr};

  // Euler angles
  sensor::Sensor *heading_{nullptr};
  sensor::Sensor *roll_{nullptr};
  sensor::Sensor *pitch_{nullptr};

  // Quaternion
  sensor::Sensor *quaternion_w_{nullptr};
  sensor::Sensor *quaternion_x_{nullptr};
  sensor::Sensor *quaternion_y_{nullptr};
  sensor::Sensor *quaternion_z_{nullptr};

  // Calibration status
  sensor::Sensor *calibration_status_{nullptr};

  // Temperature
  sensor::Sensor *temperature_{nullptr};
};

}  // namespace bno055
}  // namespace esphome
