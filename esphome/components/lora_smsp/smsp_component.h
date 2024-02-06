#include "lora/LoRa.h"
#include <Crypto.h>
#include <AES.h>
#include <GCM.h>
#include <array>
#include <vector>
#include "ReefwingMSP.h"
#include "esphome/core/component.h"
#include "esphome/components/lora/lora_component.h"

namespace esphome {
namespace lora_smsp {
AES128 aes;

void (*loraCallback)(int);
void handler(int received);

class SMSPComponent : public Component, public ReefwingMSP, public Stream {
 public:
  SMSPComponent() : ReefwingMSP(){};

  void set_lora(esphome::lora::LoRaComponent lora);
  void set_aes_key(const uint8_t *key);
  void setup() override;
  int read() override;
  int available() override;
  int availableForWrite() override;
  size_t write(uint8_t value) override;
  size_t write(uint8_t *buffer, size_t size);
  int peek() override;

 private:
  const uint8_t *key;
  esphome::lora::LoRaComponent lora;
  std::vector<uint8_t> decoded;
  std::vector<uint8_t> encoding;
  AES128 aes;
};
}  // namespace lora_smsp
}  // namespace esphome
