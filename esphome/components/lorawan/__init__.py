import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lorawan
import re
from esphome.const import CONF_ID

REGION = "region"
VERSION = "version"
DEV_EUI = "dev_eui"
APP_EUI = "app_eui"
APP_KEY = "app_key"
BOARD = "board"
JOIN_EUI = "join_eui"
NWK_KEY = "nwk_key"
MAC = "mac"
DRIVER = "driver"

lorawan_ns = cg.esphome_ns("lorawan")
Boards = lorawan_ns.enum("Boards")
BOARDS = {
    "WIRELESS_STICK_V3": Boards.WIRELESS_STICK_V3,
    "WIFI_LORA_32_V2": Boards.WIFI_LORA_32_V2,
    "WIFI_LORA_32_v3": Boards.WIFI_LORA_32_v3,
}

LoRaWanComponent = lorawan_ns.class_("LoRaWanComponent", cg.Component)


def eui(value):
    hex_string(value)
    if len(value) != 16:
        raise cv.Invalid(f"The value for EUI is invalid: '{value}'")


def hex_string(value):
    if not re.fullmatch(hex_pattern, value):
        raise cv.Invalid(
            f"This option supports only hexadecimal strings. Please remove any charcters outside of ranges 0-9, A-F, a-f from the value '{value}'"
        )


def aes128_key(value):
    hex_string(value)
    if len(value) != 32:
        raise cv.Invalid(f"The value for AES-128 key is invalid: '{value}'")


def validate_(config):
    if config[VERSION] != 1.0:
        if config[VERSION] == 1.1:
            if not JOIN_EUI in config:
                raise cv.Invalid("join_eui is required for LoRaWan 1.1")
            if not NWK_KEY in config:
                raise cv.Invalid("nwk_key is required for LoRaWan 1.1")
            return

    raise cv.Invalid(f"invalid LoRaWan version: {config[const.LORA_WAN_VERSION]}")


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateId(): cv.declare_id(LoRaWanComponent),
            cv.Required(VERSION, 1.0): cv.float_range(1, 1.1),
            cv.Required(BOARD, "no board selected"): cv.one_of(*BOARDS),
            # LoRaWan 1.0
            cv.Required(DEV_EUI, "DevEUI is not provided"): eui,
            cv.Required(APP_EUI, "AppEUI is not provided"): eui,
            cv.Required(APP_KEY, "AppKey is not provided"): aes128_key,
            # LoRaWan 1.1
            cv.Optional(JOIN_EUI): eui,
            cv.Optional(NWK_KEY): eui,
            cv.Optional("DEBUG_LEVEL", "0"): cv.int_range(0, 255),
        }
    )
    .extend(lorawan.loramac.CONFIG_SCHEMA)
    .extend(lorawan.driver.CONFIG_SCHEMA)
    .extend(lorawan.loramac.region.CONFIG_SCHEMA),
    validate_,
)

hex_pattern = re.compile(r"[A-Fa-f0-9]+")


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add_define(config[BOARD])
    cg.add_define(f"REGION_{config[REGION]}")
    cg.add_define("LoRaWAN_DEBUG_LEVEL")

    cg.add(var.set_version(config[VERSION]))
    cg.add(var.set_dev_eui(bytes.fromhex(config[DEV_EUI])))
    cg.add(var.set_app_key(bytes.fromhex(config[APP_KEY])))
    cg.add(var.set_nwk_key(bytes.fromhex(config[NWK_KEY])))
    cg.agg(var.set_join_eui(bytes.fromhex(config[JOIN_EUI])))

    yield cg.register_component(var, config)
