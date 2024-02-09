import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lora, mqtt
import re
from esphome.const import CONF_ID

msp_ns = cg.esphome_ns.namespace("mqtt_msp")
MQTTMSPComponent = msp_ns.class_("MQTTMSPComponent", cg.Component)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MQTTMSPComponent),
        }
    )
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add_library("reefwing-software/ReefwingMSP", "^2.0.0")
    cg.add_library("densaugeo/base64", "^1.4.0")
    await cg.register_component(var, config)
