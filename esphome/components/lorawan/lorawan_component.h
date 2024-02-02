#include "lorawan.h"
#include "esphome/core/component.h"

namespace esphome {
namespace lorawan {
class LoRaWanComponent : public Component {
 public:
  LoRaWanComponent();
  void set_version(const String &version);
  void set_dev_eui(const uint8_t dev_eui[]);
  void set_app_key(const uint8_t app_key[]);
  void set_nwk_key(const uint8_t nwk_key[]);
  void set_join_eui(const uint8_t join_eui[]);

  void setup() override;
  void loop() override;
}
}  // namespace lorawan
}  // namespace esphome
