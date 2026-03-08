# ERocket Component

Custom ESPHome component that integrates:
- Reed contact bike speed sensor with interrupt-based pulse counting
- Left and right SRO4M-2 UART distance sensors (Mode 1 - auto transmit)
- Motor control for automatic distance adjustment
- No interrupt conflicts - UART-based sensors don't require timing-critical operations

## Features

- **Bike Speed Measurement**: Uses reed contact on wheel with configurable wheel size
- **UART Distance Sensors**: SRO4M-2 sensors in Mode 1 automatically transmit distance data via UART
- **Motor Control**: Automatically adjusts left/right positions to maintain target distances
- **Simplified Design**: No trigger/echo timing, no interrupt disabling, reliable operation

## Configuration Example

```yaml
# UART configuration for SRO4M-2 sensors (Mode 1 - auto transmit)
uart:
  - id: left_uart
    rx_pin: GPIO16  # Left sensor UART Rx
    baud_rate: 9600
    rx_buffer_size: 256
  - id: right_uart
    rx_pin: GPIO17  # Right sensor UART Rx
    baud_rate: 9600
    rx_buffer_size: 256

# Target Distance Configuration
number:
  - platform: template
    name: "Left Side Target Distance"
    id: left_target_distance
    optimistic: true
    min_value: 10
    max_value: 100
    step: 1
    initial_value: 50
    unit_of_measurement: "cm"
    mode: slider
  - platform: template
    name: "Right Side Target Distance"
    id: right_target_distance
    optimistic: true
    min_value: 10
    max_value: 100
    step: 1
    initial_value: 50
    unit_of_measurement: "cm"
    mode: slider

external_components:
  - source: /config/custom_components
    components: [erocket]

erocket:
  id: erocket_component

  # Reed sensor configuration
  reed_pin: 27
  wheel_size: 26  # inches

  # SRO4M-2 UART distance sensors (Mode 1)
  left_uart_id: left_uart
  right_uart_id: right_uart

  # Motor control pins
  left_motor_up_pin: 18
  left_motor_down_pin: 19
  right_motor_up_pin: 21
  right_motor_down_pin: 22

  # Motor control tolerance (cm)
  tolerance: 0.8

  # Target distance references
  left_target_distance: left_target_distance
  right_target_distance: right_target_distance

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

## Pin Assignments and Wiring

### ESP32 Pins
- **Reed Sensor**: GPIO27 (INPUT_PULLUP, interrupt-driven)
- **Left Distance Sensor UART**: GPIO16 (RX only, 9600 baud)
- **Right Distance Sensor UART**: GPIO17 (RX only, 9600 baud)
- **Left Motor**: Up GPIO18, Down GPIO19
- **Right Motor**: Up GPIO21, Down GPIO22

### SRO4M-2 Sensor Wiring (CRITICAL!)
**Left Sensor:**
- Sensor VCC → 5V power
- Sensor GND → ESP32 GND (common ground required!)
- **Sensor TX → ESP32 GPIO16** (sensor transmits, ESP32 receives)
- Sensor RX → Not connected (Mode 1 doesn't need it)

**Right Sensor:**
- Sensor VCC → 5V power
- Sensor GND → ESP32 GND (common ground required!)
- **Sensor TX → ESP32 GPIO17** (sensor transmits, ESP32 receives)
- Sensor RX → Not connected (Mode 1 doesn't need it)

**Note:** Sensor TX connects to ESP32 RX. Do NOT connect sensor RX to ESP32 RX!

## How It Works

1. **Reed Sensor**: Runs continuously with hardware interrupt, counts pulses
2. **Speed Calculation**: Updates every 1 second, atomically reads pulse count and calculates km/h
3. **UART Distance Reading**:
   - SRO4M-2 sensors in Mode 1 automatically transmit distance data at ~10Hz
   - Each sensor sends 4-byte packets: `0xFF` (header), High byte, Low byte, Checksum
   - Component reads UART data asynchronously, parses packets, validates checksums
   - No trigger/echo timing required, no interrupt conflicts
   - Distance range: 2cm to 400cm
4. **Motor Control**:
   - Compares current distance to target distance every 200ms
   - Activates motor up/down to maintain distance within tolerance
   - Independent control for left and right sides

## SRO4M-2 Mode 1 Protocol

- **Baud Rate**: 9600
- **Data Format**: 4 bytes per measurement
  - Byte 0: `0xFF` (header)
  - Byte 1: High byte of distance (mm)
  - Byte 2: Low byte of distance (mm)
  - Byte 3: Checksum (sum of bytes 0-2, lower 8 bits)
- **Update Rate**: ~10Hz (automatic)
- **Range**: 20mm to 4000mm
- **No Tx Pin Required**: Sensor only transmits, no commands needed

## Advantages Over Trigger/Echo Design

- **No Interrupt Conflicts**: UART reading doesn't require disabling interrupts
- **More Reliable**: No timing-sensitive trigger/echo measurements
- **Simpler Code**: No complex pulse timing or synchronization
- **Better Range**: SRO4M-2 sensors have better performance than HC-SR04
- **Automatic Updates**: Sensors continuously transmit without polling
