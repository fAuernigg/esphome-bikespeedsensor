# ERocket Component

Custom ESPHome component that integrates:
- Reed contact bike speed sensor with interrupt-based pulse counting
- Left and right ultrasonic distance sensors (JSN-SR04T compatible)
- Interrupt management to prevent interference between systems

## Features

- **Bike Speed Measurement**: Uses reed contact on wheel with configurable wheel size
- **Ultrasonic Distance**: Measures left and right side distances with interrupt-safe timing
- **No Interference**: Disables interrupts during critical ultrasonic echo measurements

## Configuration Example

```yaml
external_components:
  - source: /config/custom_components
    components: [erocket]

erocket:
  id: erocket_component

  # Reed sensor configuration
  reed_pin: GPIO27
  wheel_size: 26  # inches

  # Left ultrasonic sensor
  left_trigger_pin: GPIO25
  left_echo_pin: GPIO26

  # Right ultrasonic sensor
  right_trigger_pin: GPIO32
  right_echo_pin: GPIO33

  # Sensor outputs
  speed_sensor:
    name: "Bike Speed"
    id: bike_speed
    filters:
      - sliding_window_moving_average:
          window_size: 3
          send_every: 1

  left_distance_sensor:
    name: "Left Side Distance"
    id: left_distance
    filters:
      - filter_out: nan
      - median:
          window_size: 5
          send_every: 2

  right_distance_sensor:
    name: "Right Side Distance"
    id: right_distance
    filters:
      - filter_out: nan
      - median:
          window_size: 5
          send_every: 2
```

## Pin Assignments

- **Reed Sensor**: GPIO27 (INPUT_PULLUP, interrupt-driven)
- **Left Ultrasonic**: Trigger GPIO25, Echo GPIO26
- **Right Ultrasonic**: Trigger GPIO32, Echo GPIO33

## How It Works

1. **Reed Sensor**: Runs continuously with hardware interrupt, counts pulses
2. **Speed Calculation**: Updates every 1 second, atomically reads pulse count and calculates km/h
3. **Ultrasonic Measurement** (Optimized for minimal interference):
   - **Alternating**: Measures left and right sensors alternately, not simultaneously
   - Sends trigger pulse (10µs) with interrupts **ENABLED**
   - Waits for echo start with interrupts **ENABLED** (allows reed pulses)
   - **Only disables interrupts during actual echo pulse timing** (2-20ms)
   - Re-enables interrupts immediately after echo measurement
   - Calculates distance in cm
   - Checks for missed reed pulses and logs warnings
4. **Loop**: Alternates between left/right ultrasonic sensors every 100ms

## Timing & Interrupt Management

- **Speed updates**: Every 1 second
- **Distance measurements**: Alternating left/right every 100ms (200ms per complete cycle)
- **Interrupt disable time**: Only 2-20ms during echo pulse measurement (not the full 25ms timeout)
- **Reed interrupt availability**: ~95% uptime (interrupts enabled during trigger, waiting, and between measurements)

This design ensures:
- Reed pulses are rarely missed (only 2-20ms blind time per 100ms)
- Ultrasonic measurements remain accurate (precise timing during echo)
- Both systems work simultaneously without significant interference
