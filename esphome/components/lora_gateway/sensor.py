import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.final_validate as fv
from esphome.components import uart

DEPENDENCIES = ["mqtt", "uart"]

lora_gateway_ns = cg.esphome_ns.namespace("lora_gateway")
LoraGatewayComponent = lora_gateway_ns.class_(
    "LoraGatewayComponent", cg.PollingComponent, uart.UARTDevice
)

CONFIG_SCHEMA = cv.All({
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LoraGatewayComponent),
            # cv.Requi
        }
    )
})
