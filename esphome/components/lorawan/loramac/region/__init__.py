import esphome.config_validation as cv
import esphome.codegen as cg

region_ns = cg.esphome_ns.namespace("lorawan.loramac.region")
Regions = region_ns.enum("Regions")


REGIONS = {
    "EU433": Regions.EU433,
    "EU868": Regions.EU868,
    "KR920": Regions.KR920,
    "AS923": Regions.AS923,
    "AS923_AS1": Regions.AS923_AS1,
    "AS923_AS2": Regions.AS923_AS2,
    "AU915": Regions.AU915,
    "CN470": Regions.CN470,
    "CN779": Regions.CN779,
    "IN865": Regions.IN865,
    "US915_HYBRID": Regions.US915_HYBRID,
    "US915": Regions.US915,
}

REGION = "region"

CONFIG_SCHEMA = cv.Schema({cv.Optional(REGION, "US915"): cv.enum(REGIONS)})
