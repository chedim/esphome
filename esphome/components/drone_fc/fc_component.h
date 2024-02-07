#pragma once
#include "esphome/components/uart/uart.h"
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
  bool recv(uint8_t &version, uint8_t &messageID, uint8_t *payload, uint8_t maxSize, uint8_t &recvSize);
  void send(uint8_t version, uint8_t messageID, uint8_t * payload, uint8_t size);
  void set_timeout(uint32_t timeout);
  bool ready() {
    return this->uart_set;
  }
 private:
  esphome::uart::UARTComponent *uart;
  bool uart_set = false;
  uint16_t roll;
  uint16_t pitch = 1500;
  uint16_t yaw = 1500;
  uint16_t throttle;
  uint8_t frame = 0;
  uint32_t timeout = 20;
};
}  // namespace drone_fc
}  // namespace esphome
