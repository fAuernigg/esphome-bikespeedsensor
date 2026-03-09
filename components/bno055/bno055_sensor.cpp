#include "bno055_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace bno055 {

static const char *TAG = "bno055";

void BNO055Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up BNO055...");

  // Check chip ID
  uint8_t chip_id;
  if (!this->read_byte(BNO055_CHIP_ID_ADDR, &chip_id)) {
    ESP_LOGE(TAG, "Failed to read chip ID!");
    this->mark_failed();
    return;
  }

  if (chip_id != 0xA0) {
    ESP_LOGE(TAG, "Invalid chip ID: 0x%02X (expected 0xA0)", chip_id);
    this->mark_failed();
    return;
  }

  ESP_LOGCONFIG(TAG, "BNO055 chip ID verified: 0x%02X", chip_id);

  // Reset the sensor
  if (!this->write_byte(BNO055_SYS_TRIGGER_ADDR, 0x20)) {
    ESP_LOGE(TAG, "Failed to reset sensor!");
    this->mark_failed();
    return;
  }

  // Wait for sensor to reset
  delay(650);

  // Set to normal power mode
  if (!this->write_byte(BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL)) {
    ESP_LOGE(TAG, "Failed to set power mode!");
    this->mark_failed();
    return;
  }

  delay(10);

  // Use internal oscillator
  if (!this->write_byte(BNO055_SYS_TRIGGER_ADDR, 0x00)) {
    ESP_LOGE(TAG, "Failed to configure oscillator!");
    this->mark_failed();
    return;
  }

  delay(10);

  // Set to NDOF mode (Nine Degrees of Freedom) for full sensor fusion
  if (!this->set_mode_(OPERATION_MODE_NDOF)) {
    ESP_LOGE(TAG, "Failed to set NDOF mode!");
    this->mark_failed();
    return;
  }

  delay(20);

  this->initialized_ = true;
  ESP_LOGCONFIG(TAG, "BNO055 initialized successfully in NDOF mode (sensor fusion enabled)");
}

bool BNO055Component::set_mode_(BNO055OperationMode mode) {
  // Switch to config mode first
  if (!this->write_byte(BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG)) {
    return false;
  }
  delay(25);

  // Now set the requested mode
  if (!this->write_byte(BNO055_OPR_MODE_ADDR, mode)) {
    return false;
  }
  delay(25);

  return true;
}

int16_t BNO055Component::read_s16_(uint8_t reg) {
  uint8_t lsb, msb;
  if (!this->read_byte(reg, &lsb) || !this->read_byte(reg + 1, &msb)) {
    return 0;
  }
  return (int16_t)((msb << 8) | lsb);
}

void BNO055Component::update() {
  if (!this->initialized_) {
    return;
  }

  // Read acceleration (raw, includes gravity) - units: m/s²
  if (this->acceleration_x_ != nullptr || this->acceleration_y_ != nullptr || this->acceleration_z_ != nullptr) {
    int16_t x = this->read_s16_(BNO055_ACCEL_DATA_X_LSB_ADDR);
    int16_t y = this->read_s16_(BNO055_ACCEL_DATA_X_LSB_ADDR + 2);
    int16_t z = this->read_s16_(BNO055_ACCEL_DATA_X_LSB_ADDR + 4);

    // Convert to m/s² (1 LSB = 1/100 m/s²)
    if (this->acceleration_x_ != nullptr) this->acceleration_x_->publish_state(x / 100.0f);
    if (this->acceleration_y_ != nullptr) this->acceleration_y_->publish_state(y / 100.0f);
    if (this->acceleration_z_ != nullptr) this->acceleration_z_->publish_state(z / 100.0f);
  }

  // Read linear acceleration (sensor fusion: without gravity) - units: m/s²
  if (this->linear_acceleration_x_ != nullptr || this->linear_acceleration_y_ != nullptr ||
      this->linear_acceleration_z_ != nullptr) {
    int16_t x = this->read_s16_(BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR);
    int16_t y = this->read_s16_(BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR + 2);
    int16_t z = this->read_s16_(BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR + 4);

    // Convert to m/s² (1 LSB = 1/100 m/s²)
    if (this->linear_acceleration_x_ != nullptr) this->linear_acceleration_x_->publish_state(x / 100.0f);
    if (this->linear_acceleration_y_ != nullptr) this->linear_acceleration_y_->publish_state(y / 100.0f);
    if (this->linear_acceleration_z_ != nullptr) this->linear_acceleration_z_->publish_state(z / 100.0f);
  }

  // Read gravity vector (sensor fusion: gravity component) - units: m/s²
  if (this->gravity_x_ != nullptr || this->gravity_y_ != nullptr || this->gravity_z_ != nullptr) {
    int16_t x = this->read_s16_(BNO055_GRAVITY_DATA_X_LSB_ADDR);
    int16_t y = this->read_s16_(BNO055_GRAVITY_DATA_X_LSB_ADDR + 2);
    int16_t z = this->read_s16_(BNO055_GRAVITY_DATA_X_LSB_ADDR + 4);

    // Convert to m/s² (1 LSB = 1/100 m/s²)
    if (this->gravity_x_ != nullptr) this->gravity_x_->publish_state(x / 100.0f);
    if (this->gravity_y_ != nullptr) this->gravity_y_->publish_state(y / 100.0f);
    if (this->gravity_z_ != nullptr) this->gravity_z_->publish_state(z / 100.0f);
  }

  // Read gyroscope - units: °/s
  if (this->gyroscope_x_ != nullptr || this->gyroscope_y_ != nullptr || this->gyroscope_z_ != nullptr) {
    int16_t x = this->read_s16_(BNO055_GYRO_DATA_X_LSB_ADDR);
    int16_t y = this->read_s16_(BNO055_GYRO_DATA_X_LSB_ADDR + 2);
    int16_t z = this->read_s16_(BNO055_GYRO_DATA_X_LSB_ADDR + 4);

    // Convert to °/s (1 LSB = 1/16 °/s)
    if (this->gyroscope_x_ != nullptr) this->gyroscope_x_->publish_state(x / 16.0f);
    if (this->gyroscope_y_ != nullptr) this->gyroscope_y_->publish_state(y / 16.0f);
    if (this->gyroscope_z_ != nullptr) this->gyroscope_z_->publish_state(z / 16.0f);
  }

  // Read magnetometer - units: µT
  if (this->magnetometer_x_ != nullptr || this->magnetometer_y_ != nullptr || this->magnetometer_z_ != nullptr) {
    int16_t x = this->read_s16_(BNO055_MAG_DATA_X_LSB_ADDR);
    int16_t y = this->read_s16_(BNO055_MAG_DATA_X_LSB_ADDR + 2);
    int16_t z = this->read_s16_(BNO055_MAG_DATA_X_LSB_ADDR + 4);

    // Convert to µT (1 LSB = 1/16 µT)
    if (this->magnetometer_x_ != nullptr) this->magnetometer_x_->publish_state(x / 16.0f);
    if (this->magnetometer_y_ != nullptr) this->magnetometer_y_->publish_state(y / 16.0f);
    if (this->magnetometer_z_ != nullptr) this->magnetometer_z_->publish_state(z / 16.0f);
  }

  // Read Euler angles (sensor fusion: absolute orientation) - units: degrees
  if (this->heading_ != nullptr || this->roll_ != nullptr || this->pitch_ != nullptr) {
    int16_t heading = this->read_s16_(BNO055_EULER_H_LSB_ADDR);
    int16_t roll = this->read_s16_(BNO055_EULER_H_LSB_ADDR + 2);
    int16_t pitch = this->read_s16_(BNO055_EULER_H_LSB_ADDR + 4);

    // Convert to degrees (1 LSB = 1/16 degree)
    if (this->heading_ != nullptr) this->heading_->publish_state(heading / 16.0f);
    if (this->roll_ != nullptr) this->roll_->publish_state(roll / 16.0f);
    if (this->pitch_ != nullptr) this->pitch_->publish_state(pitch / 16.0f);
  }

  // Read Quaternion (sensor fusion: rotation without gimbal lock)
  if (this->quaternion_w_ != nullptr || this->quaternion_x_ != nullptr ||
      this->quaternion_y_ != nullptr || this->quaternion_z_ != nullptr) {
    int16_t w = this->read_s16_(BNO055_QUATERNION_DATA_W_LSB_ADDR);
    int16_t x = this->read_s16_(BNO055_QUATERNION_DATA_W_LSB_ADDR + 2);
    int16_t y = this->read_s16_(BNO055_QUATERNION_DATA_W_LSB_ADDR + 4);
    int16_t z = this->read_s16_(BNO055_QUATERNION_DATA_W_LSB_ADDR + 6);

    // Convert to unit quaternion (1 LSB = 1/16384)
    const float scale = 1.0f / 16384.0f;
    if (this->quaternion_w_ != nullptr) this->quaternion_w_->publish_state(w * scale);
    if (this->quaternion_x_ != nullptr) this->quaternion_x_->publish_state(x * scale);
    if (this->quaternion_y_ != nullptr) this->quaternion_y_->publish_state(y * scale);
    if (this->quaternion_z_ != nullptr) this->quaternion_z_->publish_state(z * scale);
  }

  // Read calibration status
  if (this->calibration_status_ != nullptr) {
    uint8_t cal_status;
    if (this->read_byte(BNO055_CALIB_STAT_ADDR, &cal_status)) {
      // Extract individual calibration values (each is 2 bits, 0-3)
      uint8_t sys = (cal_status >> 6) & 0x03;
      uint8_t gyro = (cal_status >> 4) & 0x03;
      uint8_t accel = (cal_status >> 2) & 0x03;
      uint8_t mag = cal_status & 0x03;

      // Encode as: system*1000 + gyro*100 + accel*10 + mag
      float encoded_cal = sys * 1000.0f + gyro * 100.0f + accel * 10.0f + mag;
      this->calibration_status_->publish_state(encoded_cal);
    }
  }

  // Read temperature (°C)
  if (this->temperature_ != nullptr) {
    uint8_t temp;
    if (this->read_byte(BNO055_TEMP_ADDR, &temp)) {
      this->temperature_->publish_state((int8_t)temp);
    }
  }
}

void BNO055Component::dump_config() {
  ESP_LOGCONFIG(TAG, "BNO055 9-Axis IMU Sensor:");
  LOG_I2C_DEVICE(this);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with BNO055 failed!");
    return;
  }

  ESP_LOGCONFIG(TAG, "  Mode: NDOF (Nine Degrees of Freedom - Sensor Fusion Enabled)");
  ESP_LOGCONFIG(TAG, "  Features: Accelerometer, Gyroscope, Magnetometer");
  ESP_LOGCONFIG(TAG, "  Sensor Fusion: Euler angles, Quaternions, Linear accel, Gravity");

  LOG_UPDATE_INTERVAL(this);

  LOG_SENSOR("  ", "Acceleration X", this->acceleration_x_);
  LOG_SENSOR("  ", "Acceleration Y", this->acceleration_y_);
  LOG_SENSOR("  ", "Acceleration Z", this->acceleration_z_);

  LOG_SENSOR("  ", "Linear Acceleration X", this->linear_acceleration_x_);
  LOG_SENSOR("  ", "Linear Acceleration Y", this->linear_acceleration_y_);
  LOG_SENSOR("  ", "Linear Acceleration Z", this->linear_acceleration_z_);

  LOG_SENSOR("  ", "Gravity X", this->gravity_x_);
  LOG_SENSOR("  ", "Gravity Y", this->gravity_y_);
  LOG_SENSOR("  ", "Gravity Z", this->gravity_z_);

  LOG_SENSOR("  ", "Gyroscope X", this->gyroscope_x_);
  LOG_SENSOR("  ", "Gyroscope Y", this->gyroscope_y_);
  LOG_SENSOR("  ", "Gyroscope Z", this->gyroscope_z_);

  LOG_SENSOR("  ", "Magnetometer X", this->magnetometer_x_);
  LOG_SENSOR("  ", "Magnetometer Y", this->magnetometer_y_);
  LOG_SENSOR("  ", "Magnetometer Z", this->magnetometer_z_);

  LOG_SENSOR("  ", "Heading", this->heading_);
  LOG_SENSOR("  ", "Roll", this->roll_);
  LOG_SENSOR("  ", "Pitch", this->pitch_);

  LOG_SENSOR("  ", "Quaternion W", this->quaternion_w_);
  LOG_SENSOR("  ", "Quaternion X", this->quaternion_x_);
  LOG_SENSOR("  ", "Quaternion Y", this->quaternion_y_);
  LOG_SENSOR("  ", "Quaternion Z", this->quaternion_z_);

  LOG_SENSOR("  ", "Calibration Status", this->calibration_status_);
  LOG_SENSOR("  ", "Temperature", this->temperature_);

  // Read and log current calibration status
  uint8_t cal_status;
  if (this->read_byte(BNO055_CALIB_STAT_ADDR, &cal_status)) {
    uint8_t sys = (cal_status >> 6) & 0x03;
    uint8_t gyro = (cal_status >> 4) & 0x03;
    uint8_t accel = (cal_status >> 2) & 0x03;
    uint8_t mag = cal_status & 0x03;
    ESP_LOGCONFIG(TAG, "  Current Calibration: Sys=%d Gyro=%d Accel=%d Mag=%d", sys, gyro, accel, mag);
    ESP_LOGCONFIG(TAG, "  (3 = fully calibrated, 0 = uncalibrated)");
  }
}

}  // namespace bno055
}  // namespace esphome
