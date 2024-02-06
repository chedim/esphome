#include <AES.h>
#include "smsp_component.h"

namespace esphome {
namespace lora_smsp {

void SMSPComponent::set_lora(esphome::lora::LoRaComponent lora) { this->lora = lora; }
void SMSPComponent::set_aes_key(const uint8_t *key) { this->key = key; }
void SMSPComponent::setup() {
  this->aes.clear();
  this->aes.setKey(this->key, 16);
  this->begin(*this);
}
int SMSPComponent::peek() {
  if (this->decoded.size() == 0) {
    if (this->lora.available() > this->aes.blockSize()) {
      uint8_t *loraData;
      uint8_t *decryptedBlock;
      this->lora.readBytes(loraData, this->aes.blockSize());
      this->aes.decryptBlock(decryptedBlock, loraData);
      for (int i = 0; i < this->aes.blockSize(); i++) {
        this->decoded.insert(this->decoded.begin(), decryptedBlock[i]);
      }
    } else {
      return -1;
    }
  }

  return this->decoded.at(this->decoded.size() - 1);
}

int SMSPComponent::read() {
  int value = this->peek();
  this->decoded.pop_back();
  return value;
}

int SMSPComponent::available() { return this->decoded.size(); }

int SMSPComponent::availableForWrite() { return this->aes.blockSize() - this->encoding.size(); }

size_t SMSPComponent::write(uint8_t value) {
  this->encoding.insert(this->encoding.end(), value);
  if (this->encoding.size() == this->aes.blockSize()) {
    uint8_t *mspData;
    uint8_t *encryptedBlock;
    for (int i = 0; i < this->aes.blockSize(); i++) {
      mspData[i] = this->encoding.at(i);
    }
    this->aes.encryptBlock(encryptedBlock, mspData);
    this->lora.beginPacket();
    this->lora.write(encryptedBlock, this->aes.blockSize());
    this->lora.endPacket();
    this->encoding.clear();
  }
}

size_t SMSPComponent::write(uint8_t *buffer, size_t size) {
  for (int i = 0; i < size; i++) {
    if (this->write(buffer[i]) == -1) {
      return i - 1;
    }
  }
  return size;
}

}  // namespace lora_smsp
}  // namespace esphome
