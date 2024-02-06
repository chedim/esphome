#include "esphome/core/log.h"
#include "esphome/core/defines.h"
#include "drone_component.h"
#include "ReefwingMSP.h"


namespace esphome {
namespace drone {

#ifdef DRONE_USE_RADIO
void DroneComponent::set_radio(esphome::lora_smsp::SMSPComponent radio) { this->radio = radio; }
#endif

#ifdef DRONE_USE_CONTROLLER
void DroneComponent::set_controller(esphome::drone_fc::FCController *controller) {
  this->controller = controller;
  ESP_LOGI(TAG, "received controller object");
}
#endif

#ifdef DRONE_USE_MQTT
void DroneComponent::set_mqtt(esphome::mqtt_msp::MQTTMSPComponent *mqtt) {
  this->mqtt = mqtt;
  this->mqtt->set_drone_id(this->id_);
}
#endif

void DroneComponent::setup() {
  ESP_LOGE(TAG, "drone '%s' setup?", this->id_);
#ifdef DRONE_USE_MQTT
  this->mqtt->set_drone_id(this->id_);
//  this->mqtt->setup();
  ESP_LOGCONFIG(TAG, "Initialized drone mqtt: %s", this->id_);
#endif
  ESP_LOGE(TAG, "Initialized drone %s", this->id_);
}

void DroneComponent::loop() {
  msp_packet_t packet;
  uint8_t msgid;
  uint8_t *payload = (uint8_t *) malloc(255);
  uint8_t size;
#ifdef DRONE_USE_CONTROLLER
  // read message from controller and send it to radio and mqtt, if set
  if (this->controller->recv(&msgid, payload, 255, &size)) {
    ESP_LOGI(TAG, "Received message %d(%d) from controller: %s", msgid, size, "");
#ifdef DRONE_USE_RADIO
    this->radio->send(msgid, payload, size);
#endif
#ifdef DRONE_USE_MQTT
      ESP_LOGI(TAG, "forwarding to mqtt");
      this->mqtt->send(msgid, payload, size);
#endif
  }
#endif
#ifdef DRONE_USE_RADIO
  // read messages from radio and send it to fc and mqtt, if set
  if (this->radio->recv(&msgid, payload, 255, &size)) {
#ifdef DRONE_USE_CONTROLLER
    this->controller->send(msgid, payload, size);
#endif
#ifdef DRONE_USE_MQTT
    this->mqtt->send(msgid, payload, size);
#endif
  }
#endif
#ifdef DRONE_USE_MQTT
  // read message from mqtt and send it to radio and fc, if set
   if (this->mqtt->recv(msgid, payload, 255, size)) {
     ESP_LOGV(TAG, "mqtt msp received packet %d(%d): %s", msgid, size, payload);
#ifdef DRONE_USE_CONTROLLER
     ESP_LOGV(TAG, "forwarding to UART");
     this->controller->send(msgid, payload, size);
#endif
#ifdef DRONE_USE_RADIO
   this->radio.send(msgid, payload, size);
#endif
#ifndef DRONE_USE_CONTROLLER
#ifndef DRONE_USE_RADIO
   ESP_LOGD(TAG, "debug mode -> echoing mqtt msp packet back");
   this->mqtt->send(msgid, payload, size);
#endif
#endif
   } else {
//     ESP_LOGVV(TAG, "no new mqtt msp packets received");
   }
#endif
   free(payload);
}

void DroneComponent::dump_config() {
  this->setup();
  ESP_LOGCONFIG(TAG, "DroneComponent, id: '%s'", this->id_);
#ifdef DRONE_USE_MQTT
  ESP_LOGCONFIG(TAG, "MQTT: true");
#else
  ESP_LOGCONFIG(TAG, "MQTT: false");
#endif
#ifdef DRONE_USE_CONTROLLER
  ESP_LOGCONFIG(TAG, "Controller: true");
#else
  ESP_LOGCONFIG(TAG, "Controller: false");
#endif
#ifdef DRONE_USE_RADIO
  ESP_LOGCONFIG(TAG, "Radio: true");
#else
  ESP_LOGCONFIG(TAG, "Radio: false");
#endif
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Component failed");
  }
}
}  // namespace drone

}  // namespace esphome
