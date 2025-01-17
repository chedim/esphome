#include "fc_component.h"

void esphome::drone_fc::FCController::setup() {
#ifdef ENABLE_ROLL_SENSOR
  this->roll_sensor->set_name("roll");
#endif
#ifdef ENABLE_PITCH_SENSOR
  this->pitch_sensor->set_name("pitch");
#endif
#ifdef ENABLE_YAW_SENSOR
  this->yaw_sensor->set_name("yaw");
#endif
#ifdef ENABLE_THROTTLE_SENSOR
  this->throttle_sensor->set_name("throttle");
#endif
}

void esphome::drone_fc::FCController::loop() {
  if (this->frame++ >= 5) {
    ESP_LOGI(TAG, "sending dummy RC roll: %d, pitch: %d, yaw: %d, throttle: %d", this->roll, this->pitch, this->yaw, this->throttle);
    uint16_t payload[18];
    payload[0] = 1000 + this->roll;
    payload[1] = 1000 + this->pitch;
    payload[2] = 1000 + this->throttle;
    payload[3] = 1000 + this->yaw;
    payload[4] = 1504;
    for (int i = 5; i < 18; i++) {
      payload[i] = 1500 + i;
    }
    this->send((uint8_t)'M', (uint16_t)MSP_SET_RAW_RC, (uint8_t *)payload, (uint16_t)34);
    this->frame = 0;
    this->sent_raw_rc = true;
#ifdef ENABLE_ROLL_SENSOR
    this->roll_sensor->publish_state((payload[0] - 1000) / 5 - 100);
#endif
#ifdef ENABLE_PITCH_SENSOR
    this->pitch_sensor->publish_state((payload[1] - 1000) / 5 - 100);
#endif
#ifdef ENABLE_THROTTLE_SENSOR
    this->throttle_sensor->publish_state((payload[2] - 1000) / 10);
#endif
#ifdef ENABLE_YAW_SENSOR
    this->yaw_sensor->publish_state((payload[3] - 1000) / 5 - 100);
#endif
  }
}


void esphome::drone_fc::FCController::dump_config() {
  ESP_LOGCONFIG(TAG, "UART set: %d", this->uart_set);
  if (this->uart_set) {
    ESP_LOGCONFIG(TAG, "UART baud rate: %d", this->uart->get_baud_rate());
    ESP_LOGCONFIG(TAG, "UART TX: %d; RX: %d", this->uart->get_tx_pin()->get_pin(), this->uart->get_rx_pin()->get_pin());
  }
}


bool esphome::drone_fc::FCController::recv(uint8_t &version, uint16_t &messageID, uint8_t *payload, uint16_t maxSize, uint16_t &recvSize) {
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
          int header_size = 3;
          if (version == 'X') {
            header_size = 5;
          }
          while(this->uart->available() < header_size) {
            if (millis() - start >= this->timeout) {
              ESP_LOGW(TAG, "timed out while reading size and message id");
              return false;
            }
          }
          uint8_t checksumCalc = 0;
          if (version == 'M') {
            this->uart->read_byte((uint8_t*)&recvSize);
            ((uint8_t*)&recvSize)[1] = 0;
            this->uart->read_byte((uint8_t*) &messageID);
            ((uint8_t*)&messageID)[1] = 0;
            checksumCalc = recvSize ^ messageID;
          } else if (version == 'X') {
            uint8_t tag;
            this->uart->read_byte(&tag);
            this->crc8_dvb_s2(&checksumCalc, tag);
            this->uart->read_byte((uint8_t*) &messageID);
            this->crc8_dvb_s2(&checksumCalc, *(uint8_t*)&messageID);
            this->uart->read_byte(&((uint8_t*)&messageID)[1]);
            this->crc8_dvb_s2(&checksumCalc, ((uint8_t*)&messageID)[1]);
            this->uart->read_byte((uint8_t*)&recvSize);
            this->crc8_dvb_s2(&checksumCalc, *(uint8_t*)&recvSize);
            this->uart->read_byte(&((uint8_t*)&recvSize)[1]);
            this->crc8_dvb_s2(&checksumCalc, ((uint8_t*)&recvSize)[1]);
          } else {
            return false;
          }

          uint16_t idx = 0;
          uint8_t * payloadPtr = (uint8_t*)payload;
          while (idx < recvSize) {
            if (millis() - start > this->timeout) {
              ESP_LOGW(TAG, "timed out while reading data at position '%d/%d' (after %d ms)", idx, recvSize, millis() - start);
              return false;
            }

            if (this->uart->available() > 0) {
              uint8_t b;
              this->uart->read_byte(&b);
              if (version == 'M') {
                checksumCalc ^= b;
              } else {
                this->crc8_dvb_s2(&checksumCalc, b);
              }
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
            if (messageID == MSP_SET_RAW_RC && this->sent_raw_rc) {
              // don't spam upstream with local raw_rc data
              ESP_LOGV(TAG, "confirmed latest raw rc data");
              this->sent_raw_rc = false;
              return false;
            }
            return true;
          } else {
            ESP_LOGW(TAG, "crc checksum failed: calculated (%d) != received (%d)", checksumCalc, checksum);
          }
        } else if (header[2] == '!') {
          ESP_LOGE(TAG, "received error message: %s", header);
        }
      }
    }
  }
  return false;
}

void esphome::drone_fc::FCController::send(uint8_t version, uint16_t messageID, uint8_t * payload, uint16_t size) {
  if (!this->ready()) {
    ESP_LOGW(TAG, "uart's not ready to write");
  }
  this->uart->write_byte('$');
  this->uart->write_byte(version);
  this->uart->write_byte('<');
  if (version == 'M') {
    this->uart->write_byte(size);
    this->uart->write_byte(messageID);

    uint8_t checksum = size ^ messageID;
    uint8_t *payloadPtr = (uint8_t *) payload;

    for (uint8_t i = 0; i < size; ++i) {
      uint8_t b = *(payloadPtr++);
      checksum ^= b;
      this->uart->write_byte(b);
    }
    this->uart->write_byte(checksum);
  } else if (version == 'X') {
    uint8_t crc = 0;
    this->uart->write_byte(0);
    this->uart->write_array((uint8_t*) &messageID, 2);
    this->crc8_dvb_s2(&crc, ((uint8_t) messageID));
    this->crc8_dvb_s2(&crc, ((uint8_t*) &messageID)[1]);
    this->uart->write_array((uint8_t*)&size, 2);
    this->crc8_dvb_s2(&crc, ((uint8_t) size));
    this->crc8_dvb_s2(&crc, ((uint8_t*) &size)[1]);
    for (uint16_t i = 0; i < size; i++) {
      this->uart->write_byte(payload[i]);
      this->crc8_dvb_s2(&crc, payload[i]);
    }
    this->uart->write_byte(crc);
  }
}
