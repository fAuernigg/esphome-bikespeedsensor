#pragma once
#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace bike_speed_sensor {

class ReedSpeedSensor : public esphome::sensor::Sensor, public esphome::PollingComponent {
 public:
  int pin;
  float wheel_inch;
  float wheel_circumference;
  volatile int pulse_count = 0;
  unsigned long last_update = 0;

  ReedSpeedSensor(int reed_pin, float wheel_size_inch)
      : pin(reed_pin), wheel_inch(wheel_size_inch) {
    wheel_circumference = wheel_inch * 3.1416f * 0.0254f;  // metres
  }

  static void IRAM_ATTR on_pulse_static(void* arg) {
    reinterpret_cast<ReedSpeedSensor*>(arg)->on_pulse();
  }

  void on_pulse() { pulse_count++; }

  void setup() override {
    pinMode(pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(pin), on_pulse_static, this, FALLING);
  }

  void update() override {
    unsigned long now = millis();
    unsigned long dt = now - last_update;
    last_update = now;
    int pulses = pulse_count;
    pulse_count = 0;
    if (dt == 0) return;
    float rps = (float)pulses / (dt / 1000.0f);
    float speed_kmh = wheel_circumference * rps * 3.6f;
    publish_state(speed_kmh);
  }
};

}  // namespace bike_speed_sensor
