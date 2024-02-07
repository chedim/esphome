import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lora, mqtt
import re
from esphome.const import CONF_ID

REGION = "region"
VERSION = "version"
SECRET_KEY = "secret_key"
SS_PIN = "ss_pin"
RESET_PIN = "reset_pin"
DIO0_PIN = "dio0_pin"
DRIVER = "driver"
FREQUENCY = "frequency"
SPI_FREQUENCY = "spi_frequency"
MODE = "mode"
SYNC_WORD = "sync_word"

lora_ns = cg.esphome_ns.namespace("lora")
Boards = lora_ns.enum("Boards")
BOARDS = {
    "WIRELESS_STICK_V3": Boards.WIRELESS_STICK_V3,
    "WIFI_LORA_32_V2": Boards.WIFI_LORA_32_V2,
    "WIFI_LORA_32_v3": Boards.WIFI_LORA_32_v3,
}

Frequencies = lora_ns.enum("Frequencies")
FREQUENCIES = {
    "433": Frequencies.FREQUENCY433,
    "866": Frequencies.FREQUENCY866,
    "915": Frequencies.FREQUENCY915,
}

ModeName = lora_ns.enum("ModeName")
MODE_NAMES = {
    "END_DEVICE": ModeName.END_DEVICE,
    "GATEWAY": ModeName.GATEWAY,
}

LoRaComponent = lora_ns.class_("LoRaComponent", cg.Component)


def eui(value):
    if len(value) != 16:
        raise cv.Invalid(f"The value for EUI is invalid: '{value}'")
    return hex_string(value)


def hex_string(value):
    if not re.fullmatch(hex_pattern, value):
        raise cv.Invalid(
            f"This option supports only hexadecimal strings. Please remove any charcters outside of ranges 0-9, A-F, a-f from the value '{value}'"
        )

    return bytes.fromhex(value)


def aes128_key(value):
    if len(value) != 32:
        raise cv.Invalid(f"The value for AES-128 key is invalid: '{value}'")
    return hex_string(value)


def hex_byte(value):
    if len(value) != 2:
        raise cv.Invalid(f"The value for a hex_byte is invalid: '{value}'")

    return hex_string(value)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(LoRaComponent),
        cv.Required(MODE): cv.one_of(*MODE_NAMES),
        cv.Required(FREQUENCY): cv.one_of(*FREQUENCIES),
        cv.Required(SECRET_KEY): aes128_key,
        cv.Optional(SS_PIN, 18): cv.int_,
        cv.Optional(RESET_PIN, 14): cv.int_,
        cv.Optional(DIO0_PIN, 26): cv.int_,
        cv.Optional(SPI_FREQUENCY, 10000000): cv.int_,
        cv.Optional(SYNC_WORD, "42"): hex_byte,
    }
)

hex_pattern = re.compile(r"[A-Fa-f0-9]+")


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add_library("heltecautomation/Heltec ESP32 Dev-Boards", "1.1.2")
    cg.add_library("reefwing-software/ReefwingMSP", "^2.0.0")
    cg.add(var.set_version(config[VERSION]))
    cg.add(var.set_mode(config[MODE]))
    cg.add(var.set_secret_key(config[SECRET_KEY]))

    cg.add(var.set_frequency(config[FREQUENCY]))
    cg.add(var.set_ss_pin(config[SS_PIN]))
    cg.add(var.set_reset_pin(config[RESET_PIN]))
    cg.add(var.set_dio0_pin(config[DIO0_PIN]))
    cg.add(var.set_spi_frequency(config[SPI_FREQUENCY]))
    cg.add(var.set_sync_word(config[SYNC_WORD]))

    yield cg.register_component(var, config)
