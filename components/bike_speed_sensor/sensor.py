import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, UNIT_KILOMETER_PER_HOUR

DEPENDENCIES = ['sensor']
AUTO_LOAD = ['sensor']

CONF_REED_PIN = "reed_pin"
CONF_WHEEL_SIZE = "wheel_size"

bike_ns = cg.esphome_ns.namespace("bike_speed_sensor")

ReedSpeedSensor = bike_ns.class_("ReedSpeedSensor", cg.Component)

CONFIG_SCHEMA = sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
        icon="mdi:speedometer",
        accuracy_decimals=1,
    ).extend({
    	cv.GenerateID(): cv.declare_id(ReedSpeedSensor),
	}).extend({
        cv.Required(CONF_REED_PIN): cv.int_,
        cv.Optional(CONF_WHEEL_SIZE, default=28.0): cv.float_,
    }).extend(cv.polling_component_schema("1s")).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_REED_PIN], config[CONF_WHEEL_SIZE])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
