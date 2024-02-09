#pragma once
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#ifdef DRONE_USE_RADIO
#include "esphome/components/lora_smsp/smsp_component.h"
#endif
#ifdef DRONE_USE_CONTROLLER
#include "esphome/components/drone_fc/fc_component.h"
#endif
#ifdef DRONE_USE_MQTT
#include "esphome/components/mqtt_msp/mqttmsp_component.h"
#endif

#define MAX_MSPV2_SIZE 65535

namespace esphome {
namespace drone {
static const char *TAG = "drone";

class DroneComponent : public esphome::Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_id(char *id) {
    this->id_ = id;
    ESP_LOGCONFIG(TAG, "Got drone id: '%s'", id);
  };
#ifdef DRONE_USE_RADIO
  void set_radio(esphome::lora_smsp::SMSPComponent radio);
  esphome::lora_smsp::SMSPComponent radio;
#endif

#ifdef DRONE_USE_CONTROLLER
  void set_controller(esphome::drone_fc::FCController *controller);
  esphome::drone_fc::FCController *controller;
#endif

#ifdef DRONE_USE_MQTT
  void set_mqtt(esphome::mqtt_msp::MQTTMSPComponent *mqtt);
  char *topic;
  esphome::mqtt_msp::MQTTMSPComponent *mqtt;
#endif

 private:
  char *id_;
  uint8_t rc_rate;
  uint8_t rc_expo;
  uint8_t roll_pitch_rate;
  uint8_t yaw_rate;
  uint8_t dyn_throttle_pid;
  uint8_t throttle_mid;
  uint8_t throttle_expo;

  uint8_t msp_version;
  uint16_t msp_msgid;
  uint8_t msp_payload[MAX_MSPV2_SIZE];
  uint16_t msp_size;
};
}  // namespace drone
}  // namespace esphome
