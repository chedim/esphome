import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import uart
from esphome.const import CONF_ID, CONF_UART_ID

UART = "uart"

fc_ns = cg.esphome_ns.namespace("drone_fc")
FCController = fc_ns.class_("FCController", cg.Component, uart.UARTComponent)
uart_ns = cg.esphome_ns.namespace("uart")
UARTComponent = uart_ns.class_("UARTComponent")

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(FCController),
}).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    uart = await cg.get_variable(config[CONF_UART_ID])
    cg.add(var.set_uart(uart))
    await cg.register_component(var, config)