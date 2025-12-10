import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lora, mqtt
import re
from esphome.const import CONF_ID

lora_ppp_ns = cg.esphome_ns.namespace("lora_ppp")
LoRaPPPComponent = lora_ppp_ns.class_("LoRaPPPComponent", cg.Component)

lora_ns = cg.esphome_ns.namespace("lora")
LoRaComponent = lora_ns.class_("LoRaComponent")

LORA = "lora"
KEY = "key"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LoRaPPPComponent),
            cv.Required(LORA): cv.use_id(LoRaComponent),
        }
    )
)

hex_pattern = re.compile(r"[A-Fa-f0-9]+")


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    lora_component = cg.new_Pvariable(config[LORA])

    cg.add(var.set_lora(lora_component))
    cg.add(var.set_aes_key(config[KEY]))

    yield cg.register_component(var, config)
