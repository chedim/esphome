#pragma once
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "ReefwingMSP.h"

namespace esphome {
namespace drone_fc {
static const char *TAG = "drone_fc";
class FCController : public esphome::Component {
 public:
  void set_uart_parent(esphome::uart::UARTComponent *device) {
    this->uart = device;
    this->uart_set = true;
  };
  void dump_config() override;
  void setup() override;
  void loop() override;
  bool recv(uint8_t &version, uint16_t &messageID, uint8_t *payload, uint16_t maxSize, uint16_t &recvSize);
  void send(uint8_t version, uint16_t messageID, uint8_t * payload, uint16_t size);
  void set_timeout(uint32_t timeout);
  bool ready() {
    return this->uart_set;
  }
  void set_roll_sensor(esphome::sensor::Sensor *sensor) {
    this->roll_sensor = sensor;
  }
  void set_pitch_sensor(esphome::sensor::Sensor *sensor) {
    this->pitch_sensor = sensor;
  }
  void set_yaw_sensor(esphome::sensor::Sensor *sensor) {
    this->yaw_sensor = sensor;
  }
  void set_throttle_sensor(esphome::sensor::Sensor *sensor) {
    this->throttle_sensor = sensor;
  }
 protected:
  esphome::uart::UARTComponent *uart;
  bool uart_set = false;
  uint16_t roll = 500;
  uint16_t pitch = 1000;
  uint16_t yaw = 600;
  uint16_t throttle = 500;
  uint8_t frame = 0;
  bool sent_raw_rc = false;
  uint32_t timeout = 100;
  esphome::sensor::Sensor *roll_sensor;
  esphome::sensor::Sensor *pitch_sensor;
  esphome::sensor::Sensor *yaw_sensor;
  esphome::sensor::Sensor *throttle_sensor;
 private:
  void crc8_dvb_s2(uint8_t* crc, unsigned char a) {
    *crc ^= a;
    for (int ii = 0; ii < 8; ++ii) {
      if (*crc & 0x80) {
        *crc = (*crc << 1) ^ 0xD5;
      } else {
        *crc = *crc << 1;
      }
    }
  }

  void crc8_dvb_s2(uint8_t* crc, uint8_t* data, size_t size) {
    for (size_t i; i < size; i++) {
      this->crc8_dvb_s2(crc, data[i]);
    }
  }

};
}  // namespace drone_fc
}  // namespace esphome
