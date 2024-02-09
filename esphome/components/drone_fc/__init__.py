import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import uart, sensor
from esphome.const import (
    CONF_SENSOR,
    CONF_VOLTAGE,
    CONF_ID,
    CONF_UART_ID,
    DEVICE_CLASS_EMPTY,
    ICON_EMPTY,
    UNIT_EMPTY,
    ICON_GYROSCOPE_X,
    ICON_GYROSCOPE_Y,
    ICON_GYROSCOPE_Z,
    ICON_ACCELERATION_X,
    ICON_ACCELERATION_Y,
    ICON_ACCELERATION_Z,
    UNIT_PERCENT,
    STATE_CLASS_MEASUREMENT,
)

UART = "uart"
PITCH_SENSOR = "pitch_sensor"
ROLL_SENSOR = "roll_sensor"
YAW_SENSOR = "yaw_sensor"
THROTTLE_SENSOR = "throttle_sensor"

fc_ns = cg.esphome_ns.namespace("drone_fc")
FCController = fc_ns.class_("FCController", cg.Component, uart.UARTComponent)
uart_ns = cg.esphome_ns.namespace("uart")
UARTComponent = uart_ns.class_("UARTComponent")

pitch_roll_yaw_throttle_sensor_schema = {
    "unit_of_measurement": UNIT_PERCENT,
    "accuracy_decimals": 2,
    "state_class": STATE_CLASS_MEASUREMENT,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(FCController),
        cv.Optional(PITCH_SENSOR): sensor.sensor_schema(
            icon=ICON_EMPTY,
            **pitch_roll_yaw_throttle_sensor_schema
        ),
        cv.Optional(ROLL_SENSOR): sensor.sensor_schema(
            icon=ICON_EMPTY,
            **pitch_roll_yaw_throttle_sensor_schema
        ),
        cv.Optional(YAW_SENSOR): sensor.sensor_schema(
            icon=ICON_EMPTY,
            **pitch_roll_yaw_throttle_sensor_schema
        ),
        cv.Optional(THROTTLE_SENSOR): sensor.sensor_schema(
            icon=ICON_EMPTY,
            **pitch_roll_yaw_throttle_sensor_schema
        ),
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await uart.register_uart_device(var, config)

    if PITCH_SENSOR in config:
        cg.add_define("ENABLE_PITCH_SENSOR")
        sens = await sensor.new_sensor(config[PITCH_SENSOR])
        cg.add(var.set_pitch_sensor(sens))

    if ROLL_SENSOR in config:
        cg.add_define("ENABLE_ROLL_SENSOR")
        sens = await sensor.new_sensor(config[ROLL_SENSOR])
        cg.add(var.set_roll_sensor(sens))

    if YAW_SENSOR in config:
        cg.add_define("ENABLE_YAW_SENSOR")
        sens = await sensor.new_sensor(config[YAW_SENSOR])
        cg.add(var.set_yaw_sensor(sens))

    if THROTTLE_SENSOR in config:
        cg.add_define("ENABLE_THROTTLE_SENSOR")
        sens = await sensor.new_sensor(config[THROTTLE_SENSOR])
        cg.add(var.set_throttle_sensor(sens))
