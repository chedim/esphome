#include "lora/LoRa.h"
#include "esphome/core/component.h"
#include <cstdint>

namespace esphome {
namespace lora {

enum ModeName { END_DEVICE, GATEWAY };

struct LoRaPacket {
  /* data */
};

class LoRaComponent : public Component, public LoRaClass {
 public:
  LoRaComponent();
  void set_version(const String &version);
  void set_secret_key(const uint8_t (&app_key)[16]);

  void set_frequency(const long frequency);
  void set_ss_pin(const int ss_pin);
  void set_reset_pin(const int reset_pin);
  void set_dio0_pin(const int dio0_pin);
  void set_spi_frequency(const uint32_t frequency);
  void set_sync_word(int sync_word);
  void set_mode(const ModeName mode);

  void setup() override;
  void loop() override;

 protected:
  String version;
  ModeName mode;
  std::array<uint8_t, 16> security_key;

  long frequency;
  int ss_pin;
  int reset_pin;
  int dio0_pin;
  int sync_word;
  uint32_t spi_frequency;

  void processPacket(int packetSize);
};

}  // namespace lora
}  // namespace esphome
