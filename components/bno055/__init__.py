"""ESPHome BNO055 9-Axis IMU Sensor Component with Sensor Fusion"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@custom"]

bno055_ns = cg.esphome_ns.namespace("bno055")
BNO055Component = bno055_ns.class_("BNO055Component", cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BNO055Component),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(i2c.i2c_device_schema(0x28))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
