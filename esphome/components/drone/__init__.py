import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

RADIO = "radio"
CONTROLLER = "controller"
MQTT = "mqtt"

lora_ns = cg.esphome_ns.namespace("lora")
drone_ns = cg.esphome_ns.namespace("drone")
smsp_ns = cg.esphome_ns.namespace("lora_smsp")
fc_ns = cg.esphome_ns.namespace("drone_fc")
mqttmsp_ns = cg.esphome_ns.namespace("mqtt_msp")

DroneComponent = drone_ns.class_("DroneComponent", cg.Component)
SMSPComponent = smsp_ns.class_("SMSPComponent")
FCController = fc_ns.class_("FCController")
MQTTMSPComponent = mqttmsp_ns.class_("MQTTMSPComponent")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DroneComponent),
        cv.Optional(RADIO): cv.use_id(SMSPComponent),
        cv.Optional(CONTROLLER): cv.use_id(FCController),
        cv.Optional(MQTT): cv.use_id(MQTTMSPComponent),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add_library("reefwing-software/ReefwingMSP", "^2.0.0")
    cg.add(var.set_id(config[CONF_ID].id))
    if RADIO in config:
        radio = await cg.get_variable(config[RADIO])
        cg.add(var.set_radio(radio))
        cg.add_define("DRONE_USE_RADIO")

    if CONTROLLER in config:
        fc = await cg.get_variable(config[CONTROLLER])
        cg.add(var.set_controller(fc))
        cg.add_define("DRONE_USE_CONTROLLER")

    if MQTT in config:
        cg.add_define("DRONE_USE_MQTT")
        mqtt = await cg.get_variable(config[MQTT])
        cg.add(var.set_mqtt(mqtt))
