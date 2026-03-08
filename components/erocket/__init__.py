"""ESPHome ERocket Component - Bike Speed + UART Distance Sensors"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, number, uart
from esphome import pins
from esphome.const import (
    CONF_ID,
    UNIT_KILOMETER_PER_HOUR,
    UNIT_CENTIMETER,
)

__version__ = "2.0.0"

DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor']

# Configuration keys
CONF_REED_PIN = "reed_pin"
CONF_WHEEL_SIZE = "wheel_size"
CONF_LEFT_UART_ID = "left_uart_id"
CONF_RIGHT_UART_ID = "right_uart_id"
CONF_LEFT_MOTOR_UP_PIN = "left_motor_up_pin"
CONF_LEFT_MOTOR_DOWN_PIN = "left_motor_down_pin"
CONF_RIGHT_MOTOR_UP_PIN = "right_motor_up_pin"
CONF_RIGHT_MOTOR_DOWN_PIN = "right_motor_down_pin"
CONF_TOLERANCE = "tolerance"
CONF_SPEED_SENSOR = "speed_sensor"
CONF_LEFT_DISTANCE_SENSOR = "left_distance_sensor"
CONF_RIGHT_DISTANCE_SENSOR = "right_distance_sensor"
CONF_LEFT_TARGET_DISTANCE = "left_target_distance"
CONF_RIGHT_TARGET_DISTANCE = "right_target_distance"

erocket_ns = cg.esphome_ns.namespace("erocket")
ERocketComponent = erocket_ns.class_("ERocketComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ERocketComponent),
    cv.Required(CONF_REED_PIN): cv.int_range(min=0, max=39),
    cv.Optional(CONF_WHEEL_SIZE, default=26.0): cv.float_,
    cv.Required(CONF_LEFT_UART_ID): cv.use_id(uart.UARTComponent),
    cv.Required(CONF_RIGHT_UART_ID): cv.use_id(uart.UARTComponent),
    cv.Required(CONF_LEFT_MOTOR_UP_PIN): cv.int_range(min=0, max=39),
    cv.Required(CONF_LEFT_MOTOR_DOWN_PIN): cv.int_range(min=0, max=39),
    cv.Required(CONF_RIGHT_MOTOR_UP_PIN): cv.int_range(min=0, max=39),
    cv.Required(CONF_RIGHT_MOTOR_DOWN_PIN): cv.int_range(min=0, max=39),
    cv.Optional(CONF_TOLERANCE, default=2.0): cv.float_range(min=0.1, max=10.0),
    cv.Optional(CONF_SPEED_SENSOR): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
        icon="mdi:speedometer",
        accuracy_decimals=1,
    ),
    cv.Optional(CONF_LEFT_DISTANCE_SENSOR): sensor.sensor_schema(
        unit_of_measurement=UNIT_CENTIMETER,
        icon="mdi:arrow-expand-horizontal",
        accuracy_decimals=1,
    ),
    cv.Optional(CONF_RIGHT_DISTANCE_SENSOR): sensor.sensor_schema(
        unit_of_measurement=UNIT_CENTIMETER,
        icon="mdi:arrow-expand-horizontal",
        accuracy_decimals=1,
    ),
    cv.Optional(CONF_LEFT_TARGET_DISTANCE): cv.use_id(number.Number),
    cv.Optional(CONF_RIGHT_TARGET_DISTANCE): cv.use_id(number.Number),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_REED_PIN],
        config[CONF_WHEEL_SIZE],
        config[CONF_LEFT_MOTOR_UP_PIN],
        config[CONF_LEFT_MOTOR_DOWN_PIN],
        config[CONF_RIGHT_MOTOR_UP_PIN],
        config[CONF_RIGHT_MOTOR_DOWN_PIN],
        config[CONF_TOLERANCE],
    )
    await cg.register_component(var, config)

    # Set UART components
    left_uart = await cg.get_variable(config[CONF_LEFT_UART_ID])
    cg.add(var.set_left_uart(left_uart))

    right_uart = await cg.get_variable(config[CONF_RIGHT_UART_ID])
    cg.add(var.set_right_uart(right_uart))

    if CONF_SPEED_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_SPEED_SENSOR])
        cg.add(var.set_speed_sensor(sens))

    if CONF_LEFT_DISTANCE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_LEFT_DISTANCE_SENSOR])
        cg.add(var.set_left_distance_sensor(sens))

    if CONF_RIGHT_DISTANCE_SENSOR in config:
        sens = await sensor.new_sensor(config[CONF_RIGHT_DISTANCE_SENSOR])
        cg.add(var.set_right_distance_sensor(sens))

    if CONF_LEFT_TARGET_DISTANCE in config:
        num = await cg.get_variable(config[CONF_LEFT_TARGET_DISTANCE])
        cg.add(var.set_left_target_distance(num))

    if CONF_RIGHT_TARGET_DISTANCE in config:
        num = await cg.get_variable(config[CONF_RIGHT_TARGET_DISTANCE])
        cg.add(var.set_right_target_distance(num))
