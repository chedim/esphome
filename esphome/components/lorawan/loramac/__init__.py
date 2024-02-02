import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lorawan

CHANNELS_TX_POWER="channels_tx_power"
CHANNELS_DATA_RATE="channels_data_rate"


CHANNELS_DATA_RATE_VALUES = {
    # todo
}

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.Optional(CHANNELS_TX_POWER, 0): cv.int_,
            cv.Optional(CHANNELS_DATA_RATE, )i: cv.one_of(*CHANNELS_DATA_RATE_VALUES),
            cv.Optional(SYSTEM_MAX_RX_ERROR, ): cv.int_,
            cv.Optional(MIN_RX_SYMBOLS, 6): cv.int_,
            cv.Optional(MAX_RX_WINDOW, ): cv.int_,
            cv.Optional(RECEIVE_DELAY1, ): cv.int_,
            cv.Optional(RECEIVE_DELAY2, ): cv.int_,
            cv.Optional(JOIN_ACCEPT_DELAY1, ): cv.int_,
            cv.Optional(JoinAcceptDelay2, ): cv.int_,
            cv.Optional(CHANNELS_NB_REP, ): cv.int_,
            cv.Optional(RX1_DR_OFFSET, ): cv.int_,
            cv.Optional(RX2_CHANNEL, ): rx2_channel_params,
            cv.Optional(UPLINK_DWELL_TIME, ): cv.int_,
            cv.Optional(DOWNLINK_DWELL_TIME, ): cv.int_,
            cv.Optional(MAX_EIRP, ): cv.float_float_with_unit,
            cv.Required(ANTENNA_GAIN): cv.float_with_unit,
            cv.Optional(REPEATER_SUPPORT, false): cv.boolean
        }
    )
)
