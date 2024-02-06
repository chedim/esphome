#include "mqttmsp_component.h"
#include <algorithm>
#include <iterator>

static const char *TAG = "mqtt_msp";

void esphome::mqtt_msp::MQTTMSPComponent::setup() {
  ESP_LOGCONFIG(TAG, "Configuring mqtt_msp for drone '%s'", this->drone_id);
  this->topic_in.append("drone/");
  this->topic_in.append(this->drone_id);
  this->topic_in.append("/msp/in");

  this->topic_out.append("drone/");
  this->topic_out.append(this->drone_id);
  this->topic_out.append("/msp/out");
  ESP_LOGCONFIG(TAG, "MQTT_MSP topic in: '%s'", this->topic_in);
  ESP_LOGCONFIG(TAG, "MQTT_MSP topic out: '%s'", this->topic_out);
  this->subscribe_json(this->topic_in, &MQTTMSPComponent::on_mqtt_message);
  ESP_LOGCONFIG(TAG, "Configured mqtt_msp.");
}

void esphome::mqtt_msp::MQTTMSPComponent::on_mqtt_message(ArduinoJson::JsonObject message) {
  mqtt_msp::message data;
  data.msgid = message["msgid"];
  std::string payload = message["payload"];
  data.payload = payload;
  this->messages.insert(this->messages.begin(), data);
  ESP_LOGD(TAG, "[%s] received mqtt message (#%d in the queue) %d(%d): %s", this->drone_id, this->messages.size(), this->messages[0].msgid, this->messages[0].payload.length(), this->messages[0].payload.c_str());
}

void esphome::mqtt_msp::MQTTMSPComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT_MSP");
  // ESP_LOGCONFIG(TAG, "MQTT_MSP drone id: '%s'", this->drone_id);
}

bool esphome::mqtt_msp::MQTTMSPComponent::recv(uint8_t &messageID, uint8_t *payload, uint8_t maxSize, uint8_t &recvSize) {
  if (this->messages.size() == 0) {
    return false;
  }
  int mi = this->messages.size() -1;
  ESP_LOGD(TAG, "messages in buffer: %d", this->messages.size());
  message *message = &this->messages[mi];
  this->messages.pop_back();
  ESP_LOGD(TAG, "delivering message %d(%d): %s", message->msgid, message->payload.length(), message->payload.c_str());
  messageID = message->msgid;
  uint8_t *msg_payload = (uint8_t *) message->payload.c_str();
  std::copy(msg_payload, msg_payload+message->payload.length() + 1, payload);
  recvSize = message->payload.length();
  ESP_LOGD(TAG, "delivered message %d(%d): %s", messageID, recvSize, payload);
  return true;
}

void esphome::mqtt_msp::MQTTMSPComponent::send(uint8_t messageID, uint8_t * payload, uint8_t size) {
  publish_json(this->topic_out, [=](JsonObject message) {
    message["msgid"] = messageID;
    message["payload"] = (char *)payload;
    message["size"] = size;
  });
}
