#include "fc_component.h"

void esphome::drone_fc::FCController::setup() {
  this->begin(*this);
  this->write((const uint8_t*)"$M<", 3);
  this->write(0);
  this->write(100);
  this->write(100);
}

void esphome::drone_fc::FCController::loop() {}

int esphome::drone_fc::FCController::peek() {
  if (!this->uart_set) {
    ESP_LOGW(TAG, "unable to use UART: not set");
    return -1;
  }
  uint8_t *data;
  if (this->uart->peek_byte(data)) {
    ESP_LOGV(TAG, "peek from UART: %c", data);
    return *data;
  }
  return -1;
}

int esphome::drone_fc::FCController::read() {
  if (!this->uart_set) {
    ESP_LOGW(TAG, "unable to use UART: not set");
    return -1;
  }
  uint8_t *data;
  if (this->uart->read_byte(data)) {
    ESP_LOGV(TAG, "read from UART: %c", data);
    return *data;
  }
  return -1;
}

int esphome::drone_fc::FCController::available() {
  if (this->uart_set) {
    return this->uart->available();
  }
  ESP_LOGW(TAG, "unable to use UART: not set");
  return 0;
}

int esphome::drone_fc::FCController::availableForWrite() {
  return 0;
}

size_t esphome::drone_fc::FCController::write(uint8_t value) {
  if (this->uart_set) {
    ESP_LOGVV(TAG, "uart write: %c", value);
    this->uart->write_byte(value);
    return 1;
  }
  ESP_LOGW(TAG, "unable to use UART: not set");
  return 0;
}

size_t esphome::drone_fc::FCController::write(const uint8_t *buffer, size_t size) {
  if (this->uart_set) {
    this->uart->write_array(buffer, size);
    return size;
  }
  ESP_LOGW(TAG, "unable to use UART: not set");
  return 0;
}

void esphome::drone_fc::FCController::dump_config() {
  ESP_LOGCONFIG(TAG, "UART set: %d", this->uart_set);
}
