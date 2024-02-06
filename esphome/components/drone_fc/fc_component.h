#pragma once
#include "esphome/components/uart/uart.h"
#include "ReefwingMSP.h"

namespace esphome {
namespace drone_fc {
static const char *TAG = "drone_fc";
class FCController : public esphome::Component, public ReefwingMSP, public Stream {
 public:
  void set_uart(esphome::uart::UARTComponent *device) {
    this->uart = device;
    this->uart_set = true;
  };
  void dump_config() override;
  void setup() override;
  void loop() override;
  int peek() override;
  int read() override;
  int available() override;
  int availableForWrite() override;
  size_t write(uint8_t value) override;
  size_t write(const uint8_t *buffer, size_t size) override;
 private:
  esphome::uart::UARTComponent *uart;
  bool uart_set = false;
};
}  // namespace drone_fc
}  // namespace esphome
