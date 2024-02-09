//
// Created by Dmitrii Chechetkin on 2/9/24.
//

#ifndef ESPHOME_COMPOSITEVIDEOCOMPONENT_H
#define ESPHOME_COMPOSITEVIDEOCOMPONENT_H

namespace esphome {
namespace composite_video {

class CompositeVideoComponent {
 public:
  void set_composite_pin(uint8_t value) {
    this->pin = value;
  }
 private:
  uint8_t pin;
};

}  // namespace composite_video
}  // namespace esphome

#endif  // ESPHOME_COMPOSITEVIDEOCOMPONENT_H
