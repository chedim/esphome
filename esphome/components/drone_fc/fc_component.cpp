#include "fc_component.h"

void esphome::drone_fc::FCController::setup() {
}

void esphome::drone_fc::FCController::loop() {
  if (this->frame++ >= 10) {
    uint16_t *payload = (uint16_t *) malloc(255);
    payload[0] = 1000 + this->roll++ % 1000;
    payload[1] = this->pitch;
    payload[2] = this->yaw;
    payload[3] = 1000 + this->throttle++ % 1000;
    payload[4] = 1000;
    for (int i = 5; i < 16; i++) {
      payload[i] = 0;
    }
    this->send((uint8_t)'M', MSP_SET_RAW_RC, (uint8_t *)payload, (uint8_t)34);
    ESP_LOGI(TAG, "sent dummy RC");
    this->frame = 0;
  }
}


void esphome::drone_fc::FCController::dump_config() {
  ESP_LOGCONFIG(TAG, "UART set: %d", this->uart_set);
  if (this->uart_set) {
    ESP_LOGCONFIG(TAG, "UART baud rate: %d", this->uart->get_baud_rate());
    ESP_LOGCONFIG(TAG, "UART TX: %d; RX: %d", this->uart->get_tx_pin()->get_pin(), this->uart->get_rx_pin()->get_pin());
  }
}


bool esphome::drone_fc::FCController::recv(uint8_t &version, uint8_t &messageID, uint8_t *payload, uint8_t maxSize, uint8_t &recvSize) {
  if (!this->ready()) {
    ESP_LOGE(TAG, "uart's not ready");
    return false;
  }

  uint32_t start = millis();
  while(1) {
    uint8_t header[3];
    while(this->uart->available() < 3) {
      if (millis() - start >= this->timeout) {
        ESP_LOGVV(TAG, "timed out waiting for header, available: %d", this->uart->available());
        return false;
      }
    }

    this->uart->read_byte(&header[0]);
    if (header[0] == '$') {
      this->uart->read_byte(&header[1]);
      if (header[1] == 'M' || header[1] == 'X') {
        version = header[1];
        this->uart->read_byte(&header[2]);
        if (header[2] == '>' || header[2] == '<') {
          ESP_LOGD(TAG, "got header: %s", header);
          while(this->uart->available() < 3) {
            if (millis() - start >= this->timeout) {
              ESP_LOGW(TAG, "timed out while reading size and message id");
              return false;
            }
          }
          this->uart->read_byte(&recvSize);
          this->uart->read_byte(&messageID);

          uint8_t checksumCalc = recvSize ^ messageID;
          uint8_t idx = 0;
          uint8_t * payloadPtr = (uint8_t*)payload;
          while (idx < recvSize) {
            if (millis() - start > timeout) {
              ESP_LOGW(TAG, "timed out while reading data at position '%d'", idx);
              return false;
            }

            if (this->uart->available() > 0) {
              uint8_t b;
              this->uart->read_byte(&b);
              checksumCalc ^= b;
              if (idx < maxSize) {
                *(payloadPtr++) = b;
              }
              idx++;
            }
          }

          while (idx++ < maxSize) {
            *(payloadPtr++) = 0;
          }

         while (this->uart->available() == 0) {
            if (millis() - start > timeout) {
              ESP_LOGW(TAG, "timed out waiting for crc");
              return false;
            }
          }

          uint8_t checksum;
          this->uart->read_byte(&checksum);
          if (checksum == checksumCalc) {
            return true;
          } else {
            ESP_LOGW(TAG, "crc checksum failed");
          }
        } else if (header[2] == '!') {
          ESP_LOGE(TAG, "received error message: %s", header);
        }
      }
    }
  }
  return false;
}

void esphome::drone_fc::FCController::send(uint8_t version, uint8_t messageID, uint8_t * payload, uint8_t size) {
  if (!this->ready()) {
    ESP_LOGW(TAG, "uart's not ready to write");
  }
  this->uart->write_byte('$');
  this->uart->write_byte(version);
  this->uart->write_byte('<');
  this->uart->write_byte(size);
  this->uart->write_byte(messageID);

  uint8_t checksum = size ^ messageID;
  uint8_t * payloadPtr = (uint8_t*)payload;

  for (uint8_t i = 0; i < size; ++i) {
    uint8_t b = *(payloadPtr++);
    checksum ^= b;
    this->uart->write_byte(b);
  }
  this->uart->write_byte(checksum);
}
