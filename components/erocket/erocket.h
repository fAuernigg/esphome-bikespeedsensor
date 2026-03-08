#pragma once
#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/uart/uart.h"

namespace erocket {

class ERocketComponent : public esphome::Component {
 public:
  // Reed sensor for bike speed
  int reed_pin;
  float wheel_inch;
  float wheel_circumference;
  volatile int pulse_count = 0;
  unsigned long last_speed_update = 0;

  // UART components for ultrasonic sensors (auto serial mode)
  esphome::uart::UARTComponent *left_uart{nullptr};
  esphome::uart::UARTComponent *right_uart{nullptr};

  // UART data buffers (4-byte packets: 0xFF, HighByte, LowByte, Unknown)
  uint8_t left_buffer[4] = {0};
  uint8_t right_buffer[4] = {0};
  uint8_t left_buffer_pos = 0;
  uint8_t right_buffer_pos = 0;

  // Motor control pins
  int left_motor_up_pin;
  int left_motor_down_pin;
  int right_motor_up_pin;
  int right_motor_down_pin;

  // Motor control parameters
  float tolerance;
  unsigned long last_motor_update = 0;
  unsigned long motor_interval = 200; // ms between motor updates

  // Sensor outputs
  esphome::sensor::Sensor *speed_sensor{nullptr};
  esphome::sensor::Sensor *left_distance_sensor{nullptr};
  esphome::sensor::Sensor *right_distance_sensor{nullptr};

  // Target distance inputs
  esphome::number::Number *left_target_distance{nullptr};
  esphome::number::Number *right_target_distance{nullptr};

  ERocketComponent(int reed, float wheel_size_inch,
                   int left_up, int left_down,
                   int right_up, int right_down,
                   float tol)
      : reed_pin(reed), wheel_inch(wheel_size_inch),
        left_motor_up_pin(left_up), left_motor_down_pin(left_down),
        right_motor_up_pin(right_up), right_motor_down_pin(right_down),
        tolerance(tol) {
    wheel_circumference = wheel_inch * 3.1416f * 0.0254f;  // metres
  }

  // Reed sensor interrupt handler
  static void IRAM_ATTR on_pulse_static(void* arg) {
    reinterpret_cast<ERocketComponent*>(arg)->on_pulse();
  }

  void on_pulse() { pulse_count++; }

  void setup() override {
    // Setup reed sensor
    pinMode(reed_pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(reed_pin), on_pulse_static, this, FALLING);

    // Setup motor control pins
    pinMode(left_motor_up_pin, OUTPUT);
    pinMode(left_motor_down_pin, OUTPUT);
    pinMode(right_motor_up_pin, OUTPUT);
    pinMode(right_motor_down_pin, OUTPUT);

    // Initialize motors off
    digitalWrite(left_motor_up_pin, LOW);
    digitalWrite(left_motor_down_pin, LOW);
    digitalWrite(right_motor_up_pin, LOW);
    digitalWrite(right_motor_down_pin, LOW);
  }

  void dump_config() override {
    ESP_LOGCONFIG("erocket", "ERocket Component");
  }

  void loop() override {
    unsigned long now = millis();

    // Update speed calculation every second
    if (now - last_speed_update >= 1000) {
      update_speed();
      last_speed_update = now;
    }

    // Read UART data from left sensor
    if (left_uart != nullptr) {
      while (left_uart->available()) {
        uint8_t byte;
        left_uart->read_byte(&byte);

        if (byte == 0xFF && left_buffer_pos == 0) {
          left_buffer[left_buffer_pos++] = byte;
        } else if (left_buffer_pos > 0 && left_buffer_pos < 4) {
          left_buffer[left_buffer_pos++] = byte;

          if (left_buffer_pos == 4) {
            float distance = parse_sro4m_packet(left_buffer, "Left");
            if (distance > 0 && left_distance_sensor != nullptr) {
              left_distance_sensor->publish_state(distance);
            }
            left_buffer_pos = 0;
          }
        } else {
          left_buffer_pos = 0;
        }
      }
    }

    // Read UART data from right sensor
    if (right_uart != nullptr) {
      while (right_uart->available()) {
        uint8_t byte;
        right_uart->read_byte(&byte);

        if (byte == 0xFF && right_buffer_pos == 0) {
          right_buffer[right_buffer_pos++] = byte;
        } else if (right_buffer_pos > 0 && right_buffer_pos < 4) {
          right_buffer[right_buffer_pos++] = byte;

          if (right_buffer_pos == 4) {
            float distance = parse_sro4m_packet(right_buffer, "Right");
            if (distance > 0 && right_distance_sensor != nullptr) {
              right_distance_sensor->publish_state(distance);
            }
            right_buffer_pos = 0;
          }
        } else {
          right_buffer_pos = 0;
        }
      }
    }

    // Update motor control
    if (now - last_motor_update >= motor_interval) {
      update_motor_control();
      last_motor_update = now;
    }
  }

  void update_speed() {
    unsigned long now = millis();
    unsigned long dt = now - last_speed_update;

    // Read pulse count atomically
    noInterrupts();
    int pulses = pulse_count;
    pulse_count = 0;
    interrupts();

    if (dt == 0) return;

    float rps = (float)pulses / (dt / 1000.0f);
    float speed_kmh = wheel_circumference * rps * 3.6f;

    if (speed_sensor != nullptr) {
      speed_sensor->publish_state(speed_kmh);
    }
  }

  float parse_sro4m_packet(uint8_t *buffer, const char *sensor_name) {
    if (buffer[0] != 0xFF) {
      return -1;
    }

    // Extract distance (in millimeters)
    uint16_t distance_mm = (buffer[1] << 8) | buffer[2];
    float distance_cm = distance_mm / 10.0f;

    // Sanity check (20mm to 4000mm range)
    if (distance_cm < 22.4f || distance_cm > 200.0f) {
      return -1;
    }

    return distance_cm;
  }

  void update_motor_control() {
    // Left side motor control
    if (left_distance_sensor != nullptr && left_target_distance != nullptr) {
      float current = left_distance_sensor->state;
      float target = left_target_distance->state;

      if (!isnan(current) && !isnan(target)) {
        if (current < target - tolerance) {
          // Need to move up (increase distance)
          digitalWrite(left_motor_down_pin, LOW);
          digitalWrite(left_motor_up_pin, HIGH);
        } else if (current > target + tolerance) {
          // Need to move down (decrease distance)
          digitalWrite(left_motor_up_pin, LOW);
          digitalWrite(left_motor_down_pin, HIGH);
        } else {
          // Within tolerance, stop motors
          digitalWrite(left_motor_up_pin, LOW);
          digitalWrite(left_motor_down_pin, LOW);
        }
      }
    }

    // Right side motor control
    if (right_distance_sensor != nullptr && right_target_distance != nullptr) {
      float current = right_distance_sensor->state;
      float target = right_target_distance->state;

      if (!isnan(current) && !isnan(target)) {
        if (current < target - tolerance) {
          // Need to move up (increase distance)
          digitalWrite(right_motor_down_pin, LOW);
          digitalWrite(right_motor_up_pin, HIGH);
        } else if (current > target + tolerance) {
          // Need to move down (decrease distance)
          digitalWrite(right_motor_up_pin, LOW);
          digitalWrite(right_motor_down_pin, HIGH);
        } else {
          // Within tolerance, stop motors
          digitalWrite(right_motor_up_pin, LOW);
          digitalWrite(right_motor_down_pin, LOW);
        }
      }
    }
  }

  void set_speed_sensor(esphome::sensor::Sensor *sensor) { speed_sensor = sensor; }
  void set_left_distance_sensor(esphome::sensor::Sensor *sensor) { left_distance_sensor = sensor; }
  void set_right_distance_sensor(esphome::sensor::Sensor *sensor) { right_distance_sensor = sensor; }
  void set_left_target_distance(esphome::number::Number *number) { left_target_distance = number; }
  void set_right_target_distance(esphome::number::Number *number) { right_target_distance = number; }
  void set_left_uart(esphome::uart::UARTComponent *uart) { left_uart = uart; }
  void set_right_uart(esphome::uart::UARTComponent *uart) { right_uart = uart; }
};

}  // namespace erocket
