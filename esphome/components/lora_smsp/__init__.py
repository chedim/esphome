import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lora, mqtt
import re
from esphome.const import CONF_ID

smsp_ns = cg.esphome_ns.namespace("lora_smsp")
SMSPComponent = smsp_ns.class_("SMSPComponent", cg.Component)

lora_ns = cg.esphome_ns.namespace("lora")
LoRaComponent = lora_ns.class_("LoRaComponent")

LORA = "lora"
KEY = "key"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SMSPComponent),
            cv.Required(LORA): cv.use_id(LoRaComponent),
            cv.Required(KEY): lora.aes128_key,
        }
    )
)

hex_pattern = re.compile(r"[A-Fa-f0-9]+")


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    lora = cg.new_Pvariable(config[LORA])

    cg.add_library("heltecautomation/Heltec ESP32 Dev-Boards", "1.1.2")
    cg.add_library("reefwing-software/ReefwingMSP", "^2.0.0")
    cg.add(var.set_lora(lora))
    cg.add(var.set_aes_key(config[KEY]))

    yield cg.register_component(var, config)
