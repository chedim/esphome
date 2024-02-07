#pragma once
#include "esphome/core/component.h"
#include "esphome/components/mqtt/custom_mqtt_device.h"
#include "ReefwingMSP.h"

namespace esphome {
namespace mqtt_msp {
struct message {
  uint8_t msgid;
  std::string payload;
  uint8_t version;
};

class MQTTMSPComponent : public esphome::Component, public mqtt::CustomMQTTDevice {
 public:
  void setup() override;
  void dump_config() override;
  void on_mqtt_message(ArduinoJson::JsonObject message);
  bool recv(uint8_t &version, uint8_t &messageID, uint8_t *payload, uint8_t maxSize, uint8_t &recvSize);
  void send(uint8_t version, uint8_t messageID, uint8_t *payload, uint8_t size);
  void set_drone_id(char *id) {
      this->drone_id = id;
  };

 private:
  char *drone_id;
  std::string topic_in;
  std::string topic_out;
  std::vector<struct message> messages;
  size_t message_position = 0;
  uint8_t current_sending_msp_size = 0;
  std::vector<uint8_t> current_sending_msp_payload;
  bool dollar_sign;
  char message_version = 0;
  char message_type = 0;
  uint8_t message_id = 0;
};
}  // namespace mqtt_msp
}  // namespace esphome
