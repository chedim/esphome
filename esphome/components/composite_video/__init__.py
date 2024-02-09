import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import pins
from esphome.const import CONF_ID, CONF_UART_ID

composite_video_ns = cg.esphome_ns.namespace("composite_video")
CompositeVideoComponent = composite_video_ns.class_("CompositeVideoComponent", cg.Component);

COMPOSITE_PIN = "composite_pin"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CompositeVideoComponent),
    cv.Required(COMPOSITE_PIN): pins.gpio_input_pin_schema
})

# async def to_code(config):

