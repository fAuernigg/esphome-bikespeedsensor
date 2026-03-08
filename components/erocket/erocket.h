#pragma once
#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"

namespace erocket {

class ERocketComponent : public esphome::Component {
 public:
  // Reed sensor for bike speed
  int reed_pin;
  float wheel_inch;
  float wheel_circumference;
  volatile int pulse_count = 0;
  unsigned long last_speed_update = 0;

  // Ultrasonic sensors
  int left_trigger_pin;
  int left_echo_pin;
  int right_trigger_pin;
  int right_echo_pin;

  // Motor control pins
  int left_motor_up_pin;
  int left_motor_down_pin;
  int right_motor_up_pin;
  int right_motor_down_pin;

  // Motor control parameters
  float tolerance;
  unsigned long last_motor_update = 0;
  unsigned long motor_interval = 200; // ms between motor updates

  // Timing control
  unsigned long last_ultrasonic_update = 0;
  unsigned long ultrasonic_interval = 100; // ms between measurements
  bool measure_left_next = true; // Alternate between sensors

  // Sensor outputs
  esphome::sensor::Sensor *speed_sensor{nullptr};
  esphome::sensor::Sensor *left_distance_sensor{nullptr};
  esphome::sensor::Sensor *right_distance_sensor{nullptr};

  // Target distance inputs
  esphome::number::Number *left_target_distance{nullptr};
  esphome::number::Number *right_target_distance{nullptr};

  ERocketComponent(int reed, float wheel_size_inch,
                   int left_trig, int left_echo,
                   int right_trig, int right_echo,
                   int left_up, int left_down,
                   int right_up, int right_down,
                   float tol)
      : reed_pin(reed), wheel_inch(wheel_size_inch),
        left_trigger_pin(left_trig), left_echo_pin(left_echo),
        right_trigger_pin(right_trig), right_echo_pin(right_echo),
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

    // Setup ultrasonic pins
    pinMode(left_trigger_pin, OUTPUT);
    pinMode(left_echo_pin, INPUT);
    pinMode(right_trigger_pin, OUTPUT);
    pinMode(right_echo_pin, INPUT);

    digitalWrite(left_trigger_pin, LOW);
    digitalWrite(right_trigger_pin, LOW);

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

  void loop() override {
    unsigned long now = millis();

    // Update speed calculation every second
    if (now - last_speed_update >= 1000) {
      update_speed();
      last_speed_update = now;
    }

    // Update ultrasonic sensors alternating (not simultaneously)
    // This reduces interrupt-disable time by 50%
    if (now - last_ultrasonic_update >= ultrasonic_interval) {
      if (measure_left_next) {
        float left_distance = measure_distance(left_trigger_pin, left_echo_pin);
        if (left_distance_sensor != nullptr && left_distance > 0) {
          left_distance_sensor->publish_state(left_distance);
        }
      } else {
        float right_distance = measure_distance(right_trigger_pin, right_echo_pin);
        if (right_distance_sensor != nullptr && right_distance > 0) {
          right_distance_sensor->publish_state(right_distance);
        }
      }
      measure_left_next = !measure_left_next;
      last_ultrasonic_update = now;
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

  float measure_distance(int trigger_pin, int echo_pin) {
    // Store reed pin state before measurement
    int reed_state_before = digitalRead(reed_pin);

    // Send trigger pulse (interrupts enabled, this is not timing-critical)
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    // Wait for echo to start (interrupts enabled)
    unsigned long timeout = 25000; // 25ms timeout (~4m range)
    unsigned long start = micros();

    while (digitalRead(echo_pin) == LOW) {
      if (micros() - start > timeout) {
        return -1; // Timeout
      }
      // Yield briefly to allow interrupts
      if ((micros() - start) % 1000 == 0) {
        delayMicroseconds(1);
      }
    }

    // Echo started - NOW disable interrupts for precise timing
    noInterrupts();
    unsigned long pulse_start = micros();

    // Measure echo pulse duration with interrupts disabled
    while (digitalRead(echo_pin) == HIGH) {
      if (micros() - pulse_start > timeout) {
        interrupts();
        return -1; // Timeout
      }
    }
    unsigned long pulse_end = micros();

    // Re-enable interrupts immediately
    interrupts();

    // Check if we might have missed a reed pulse during measurement
    int reed_state_after = digitalRead(reed_pin);
    unsigned long measurement_time = pulse_end - pulse_start;

    // If reed pin changed state and measurement took >5ms, we might have missed a pulse
    // This is a safety check - in practice, interrupts are only disabled for 2-20ms
    if (reed_state_before != reed_state_after && measurement_time > 5000) {
      ESP_LOGW("erocket", "Possible missed pulse during ultrasonic measurement (%lu us)", measurement_time);
    }

    // Calculate distance in cm
    unsigned long duration = pulse_end - pulse_start;
    float distance = duration * 0.034f / 2.0f; // Speed of sound: 340m/s

    // Sanity check (2cm to 400cm range for JSN-SR04T)
    if (distance < 2.0f || distance > 400.0f) {
      return -1;
    }

    return distance;
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
};

}  // namespace erocket
