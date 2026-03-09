"""BNO055 Sensor Platform"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_DEGREES,
    UNIT_METER_PER_SECOND_SQUARED,
    UNIT_DEGREE_PER_SECOND,
    UNIT_MICROTESLA,
)
from . import BNO055Component, bno055_ns

DEPENDENCIES = ["bno055"]

# Acceleration
CONF_ACCELERATION_X = "acceleration_x"
CONF_ACCELERATION_Y = "acceleration_y"
CONF_ACCELERATION_Z = "acceleration_z"

# Linear Acceleration (without gravity)
CONF_LINEAR_ACCELERATION_X = "linear_acceleration_x"
CONF_LINEAR_ACCELERATION_Y = "linear_acceleration_y"
CONF_LINEAR_ACCELERATION_Z = "linear_acceleration_z"

# Gravity Vector
CONF_GRAVITY_X = "gravity_x"
CONF_GRAVITY_Y = "gravity_y"
CONF_GRAVITY_Z = "gravity_z"

# Gyroscope
CONF_GYROSCOPE_X = "gyroscope_x"
CONF_GYROSCOPE_Y = "gyroscope_y"
CONF_GYROSCOPE_Z = "gyroscope_z"

# Magnetometer
CONF_MAGNETOMETER_X = "magnetometer_x"
CONF_MAGNETOMETER_Y = "magnetometer_y"
CONF_MAGNETOMETER_Z = "magnetometer_z"

# Euler Angles (sensor fusion output)
CONF_HEADING = "heading"
CONF_ROLL = "roll"
CONF_PITCH = "pitch"

# Quaternion (sensor fusion output)
CONF_QUATERNION_W = "quaternion_w"
CONF_QUATERNION_X = "quaternion_x"
CONF_QUATERNION_Y = "quaternion_y"
CONF_QUATERNION_Z = "quaternion_z"

# Calibration status
CONF_CALIBRATION_STATUS = "calibration_status"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(BNO055Component),
        # Acceleration sensors
        cv.Optional(CONF_ACCELERATION_X): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ACCELERATION_Y): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ACCELERATION_Z): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Linear acceleration sensors (without gravity)
        cv.Optional(CONF_LINEAR_ACCELERATION_X): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_LINEAR_ACCELERATION_Y): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_LINEAR_ACCELERATION_Z): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Gravity vector
        cv.Optional(CONF_GRAVITY_X): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_GRAVITY_Y): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_GRAVITY_Z): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Gyroscope sensors
        cv.Optional(CONF_GYROSCOPE_X): sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREE_PER_SECOND,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_GYROSCOPE_Y): sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREE_PER_SECOND,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_GYROSCOPE_Z): sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREE_PER_SECOND,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Magnetometer sensors
        cv.Optional(CONF_MAGNETOMETER_X): sensor.sensor_schema(
            unit_of_measurement=UNIT_MICROTESLA,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAGNETOMETER_Y): sensor.sensor_schema(
            unit_of_measurement=UNIT_MICROTESLA,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAGNETOMETER_Z): sensor.sensor_schema(
            unit_of_measurement=UNIT_MICROTESLA,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Euler angles (sensor fusion output)
        cv.Optional(CONF_HEADING): sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREES,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:compass",
        ),
        cv.Optional(CONF_ROLL): sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREES,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:axis-arrow",
        ),
        cv.Optional(CONF_PITCH): sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREES,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:axis-arrow",
        ),
        # Quaternion (sensor fusion output)
        cv.Optional(CONF_QUATERNION_W): sensor.sensor_schema(
            accuracy_decimals=4,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_QUATERNION_X): sensor.sensor_schema(
            accuracy_decimals=4,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_QUATERNION_Y): sensor.sensor_schema(
            accuracy_decimals=4,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_QUATERNION_Z): sensor.sensor_schema(
            accuracy_decimals=4,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Calibration status
        cv.Optional(CONF_CALIBRATION_STATUS): sensor.sensor_schema(
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:check-circle",
        ),
        # Temperature
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ID])

    # Acceleration sensors
    if CONF_ACCELERATION_X in config:
        sens = await sensor.new_sensor(config[CONF_ACCELERATION_X])
        cg.add(parent.set_acceleration_x_sensor(sens))
    if CONF_ACCELERATION_Y in config:
        sens = await sensor.new_sensor(config[CONF_ACCELERATION_Y])
        cg.add(parent.set_acceleration_y_sensor(sens))
    if CONF_ACCELERATION_Z in config:
        sens = await sensor.new_sensor(config[CONF_ACCELERATION_Z])
        cg.add(parent.set_acceleration_z_sensor(sens))

    # Linear acceleration sensors
    if CONF_LINEAR_ACCELERATION_X in config:
        sens = await sensor.new_sensor(config[CONF_LINEAR_ACCELERATION_X])
        cg.add(parent.set_linear_acceleration_x_sensor(sens))
    if CONF_LINEAR_ACCELERATION_Y in config:
        sens = await sensor.new_sensor(config[CONF_LINEAR_ACCELERATION_Y])
        cg.add(parent.set_linear_acceleration_y_sensor(sens))
    if CONF_LINEAR_ACCELERATION_Z in config:
        sens = await sensor.new_sensor(config[CONF_LINEAR_ACCELERATION_Z])
        cg.add(parent.set_linear_acceleration_z_sensor(sens))

    # Gravity vector
    if CONF_GRAVITY_X in config:
        sens = await sensor.new_sensor(config[CONF_GRAVITY_X])
        cg.add(parent.set_gravity_x_sensor(sens))
    if CONF_GRAVITY_Y in config:
        sens = await sensor.new_sensor(config[CONF_GRAVITY_Y])
        cg.add(parent.set_gravity_y_sensor(sens))
    if CONF_GRAVITY_Z in config:
        sens = await sensor.new_sensor(config[CONF_GRAVITY_Z])
        cg.add(parent.set_gravity_z_sensor(sens))

    # Gyroscope sensors
    if CONF_GYROSCOPE_X in config:
        sens = await sensor.new_sensor(config[CONF_GYROSCOPE_X])
        cg.add(parent.set_gyroscope_x_sensor(sens))
    if CONF_GYROSCOPE_Y in config:
        sens = await sensor.new_sensor(config[CONF_GYROSCOPE_Y])
        cg.add(parent.set_gyroscope_y_sensor(sens))
    if CONF_GYROSCOPE_Z in config:
        sens = await sensor.new_sensor(config[CONF_GYROSCOPE_Z])
        cg.add(parent.set_gyroscope_z_sensor(sens))

    # Magnetometer sensors
    if CONF_MAGNETOMETER_X in config:
        sens = await sensor.new_sensor(config[CONF_MAGNETOMETER_X])
        cg.add(parent.set_magnetometer_x_sensor(sens))
    if CONF_MAGNETOMETER_Y in config:
        sens = await sensor.new_sensor(config[CONF_MAGNETOMETER_Y])
        cg.add(parent.set_magnetometer_y_sensor(sens))
    if CONF_MAGNETOMETER_Z in config:
        sens = await sensor.new_sensor(config[CONF_MAGNETOMETER_Z])
        cg.add(parent.set_magnetometer_z_sensor(sens))

    # Euler angles
    if CONF_HEADING in config:
        sens = await sensor.new_sensor(config[CONF_HEADING])
        cg.add(parent.set_heading_sensor(sens))
    if CONF_ROLL in config:
        sens = await sensor.new_sensor(config[CONF_ROLL])
        cg.add(parent.set_roll_sensor(sens))
    if CONF_PITCH in config:
        sens = await sensor.new_sensor(config[CONF_PITCH])
        cg.add(parent.set_pitch_sensor(sens))

    # Quaternion
    if CONF_QUATERNION_W in config:
        sens = await sensor.new_sensor(config[CONF_QUATERNION_W])
        cg.add(parent.set_quaternion_w_sensor(sens))
    if CONF_QUATERNION_X in config:
        sens = await sensor.new_sensor(config[CONF_QUATERNION_X])
        cg.add(parent.set_quaternion_x_sensor(sens))
    if CONF_QUATERNION_Y in config:
        sens = await sensor.new_sensor(config[CONF_QUATERNION_Y])
        cg.add(parent.set_quaternion_y_sensor(sens))
    if CONF_QUATERNION_Z in config:
        sens = await sensor.new_sensor(config[CONF_QUATERNION_Z])
        cg.add(parent.set_quaternion_z_sensor(sens))

    # Calibration status
    if CONF_CALIBRATION_STATUS in config:
        sens = await sensor.new_sensor(config[CONF_CALIBRATION_STATUS])
        cg.add(parent.set_calibration_status_sensor(sens))

    # Temperature
    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(parent.set_temperature_sensor(sens))
