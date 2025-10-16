#include "esphome.h"

namespace bike_speed_sensor {

class ReedSpeedSensor : public PollingComponent, public Sensor {
 public:
  int pin;
  volatile int pulse_count = 0;
  unsigned long last_update = 0;
  float wheel_inch;
  float wheel_circumference;

  ReedSpeedSensor(int reed_pin, float wheel_size_inch)
      : pin(reed_pin), wheel_inch(wheel_size_inch) {
    float diameter_m = wheel_inch * 0.0254;
    wheel_circumference = 3.1416 * diameter_m;
  }

  void setup() override {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin),
                    std::bind(&ReedSpeedSensor::onPulse, this), FALLING);
  }

  void onPulse() { pulse_count++; }

  void update() override {
    unsigned long now = millis();
    unsigned long dt = now - last_update;
    last_update = now;
    int pulses = pulse_count;
    pulse_count = 0;
    if (dt == 0) return;
    float rps = (float)pulses / (dt / 1000.0);
    float speed_kmh = wheel_circumference * rps * 3.6;
    publish_state(speed_kmh);
  }
};

}  // namespace bike_speed_sensor
