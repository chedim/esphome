#include <Arduino.h>
#include "lora_component.h"
#include "lora/LoRa.h"
#include <array>
#include <algorithm>

namespace esphome {
namespace lora {

LoRaComponent::LoRaComponent() {}
void LoRaComponent::set_version(const String &version) { this->version = version; }
void LoRaComponent::set_frequency(const long frequency) { this->frequency = frequency; }
void LoRaComponent::set_ss_pin(const int ss_pin) { this->ss_pin = ss_pin; }
void LoRaComponent::set_reset_pin(const int reset_pin) { this->reset_pin = reset_pin; }
void LoRaComponent::set_dio0_pin(const int dio0_pin) { this->dio0_pin = dio0_pin; }
void LoRaComponent::set_spi_frequency(const uint32_t frequency) { this->spi_frequency = frequency; }
void LoRaComponent::set_sync_word(int sync_word) { this->sync_word = sync_word; }
void LoRaComponent::set_mode(const ModeName mode) { this->mode = mode; }
void LoRaComponent::setup() {
  this->begin(this->frequency, false);
  this->setPins(this->ss_pin, this->reset_pin, this->dio0_pin);
  this->setSPIFrequency(this->spi_frequency);
  this->setSyncWord(this->sync_word);
}

}  // namespace lora
}  // namespace esphome
