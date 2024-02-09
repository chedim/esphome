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
#ifdef DRONE_USE_CONTROLLER
  if (this->controller->ready()) {
    this->controller->loop();
    // read message from controller and send it to radio and mqtt, if set
    if (this->controller->recv(this->msp_version, this->msp_msgid, this->msp_payload, MAX_MSPV2_SIZE, this->msp_size)) {
      std::string payloadStr((const char *) this->msp_payload, this->msp_size);
      ESP_LOGI(TAG, "Received msp (v: %c) message %d(%d) from controller: %s", this->msp_version, this->msp_msgid, this->msp_size, payloadStr);
#ifdef DRONE_USE_RADIO
      this->radio->send(this->msp_version, this->msp_msgid, this->msp_payload, this->msp_size);
#endif
#ifdef DRONE_USE_MQTT
      ESP_LOGI(TAG, "forwarding to mqtt");
      this->mqtt->send(this->msp_version, this->msp_msgid, this->msp_payload, this->msp_size);
#endif
    }
  }
#endif
#ifdef DRONE_USE_RADIO
  // read messages from radio and send it to fc and mqtt, if set
  if (this->radio->recv(this->msp_version, this->msp_msgid, this->msp_payload, MAX_MSPV2_SIZE, this->msp_size)) {
#ifdef DRONE_USE_CONTROLLER
    if (this->controller->ready()) {
      ESP_LOGV(TAG, "forwarding to UART");
      this->controller->send(this->msp_version, this->msp_msgid, this->msp_payload, this->msp_size);
    } else {
      ESP_LOGW(TAG, "dropping MSP packet: controller not ready");
    }
#endif
#ifdef DRONE_USE_MQTT
    this->mqtt->send(this->msp_version, this->msp_msgid, this->msp_payload, this->msp_size);
#endif
  }
#endif
#ifdef DRONE_USE_MQTT
  // read message from mqtt and send it to radio and fc, if set
   if (this->mqtt->recv(this->msp_version, this->msp_msgid, this->msp_payload, 255, this->msp_size)) {
     ESP_LOGV(TAG, "mqtt msp (v: %c) received packet %d(%d): %s", this->msp_version, this->msp_msgid, this->msp_size, this->msp_payload);
#ifdef DRONE_USE_CONTROLLER
     if (this->controller->ready()) {
       ESP_LOGV(TAG, "forwarding to UART");
       this->controller->send(this->msp_version, this->msp_msgid, this->msp_payload, this->msp_size);
     } else {
       ESP_LOGW(TAG, "dropping MSP packet: controller not ready");
     }
#endif
#ifdef DRONE_USE_RADIO
   this->radio.send(this->msp_version, this->msp_msgid, this->msp_payload, this->msp_size);
#endif
#ifndef DRONE_USE_CONTROLLER
#ifndef DRONE_USE_RADIO
   ESP_LOGD(TAG, "debug mode -> echoing mqtt msp packet back");
   this->mqtt->send(this->msp_version, this->msp_msgid, this->msp_payload, this->msp_size);
#endif
#endif
   } else {
//     ESP_LOGVV(TAG, "no new mqtt msp packets received");
   }
#endif
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
