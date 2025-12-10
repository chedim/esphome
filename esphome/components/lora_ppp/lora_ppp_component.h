#include "lora/LoRa.h"
#include <Crypto.h>
#include <AES.h>
#include <GCM.h>
#include <array>
#include <vector>
#include "ReefwingMSP.h"
#include "esphome/core/component.h"
#include "esphome/components/lora/lora_component.h"

#ifdef USE_ESP32
#include "esp_netif.h"

namespace esphome {
namespace lora_smsp {

typedef struct netif_driver_s {
  esp_netif_driver_base_t base; /*!< base structure reserved as esp-netif driver */
  driver_impl *h;               /*!< handle of driver implementation */
} netif_driver_t;

void (*loraCallback)(int);
void handler(int received);

class SMSPComponent : public Component, public ReefwingMSP, public Stream {
 public:
  SMSPComponent() : Component(){};

  void set_lora(esphome::lora::LoRaComponent lora) { this->lora = lora; };
  void setup() override {
    esp_err_t err;
    err = esp_netif_attach(this->netif, this->driver);
  };
  void loop() override;

 private:
  esphome::lora::LoRaComponent lora;
  netif_driver_t driver;
};
}  // namespace lora_smsp
}  // namespace esphome

#endif
