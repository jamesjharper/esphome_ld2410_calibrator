#include "ld2410_calibrator_number.h"

#include "esphome/core/log.h"

namespace esphome {
namespace ld2410_calibrator {

static const char *const TAG = "ld2410_calibrator.number";


optional<float> LD2410CalibratorNumber::get_initial_value() { return this->initial_value_; }

void LD2410CalibratorNumber::setup() {
  float value;
  if (!this->restore_value_) {
    if (this->initial_value_.has_value()) {
      value = *this->initial_value_;
    } else {
      value = this->traits.get_min_value();
    }
  } else {
    this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
    if (!this->pref_.load(&value)) {
      if (this->initial_value_.has_value()) {
        value = *this->initial_value_;
      } else {
        value = this->traits.get_min_value();
      }
    }
  }
  this->publish_state(value);
  this->set_action(value);
}

void LD2410CalibratorNumber::dump_config() {
  LOG_NUMBER("", "LD2410 Calibrator Number", this);
  ESP_LOGCONFIG(TAG, "  Restore value: %s", YESNO(this->restore_value_));
  if (this->initial_value_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Initial value: %.2f", *this->initial_value_);
  }
}

}  // namespace ld2410_calibrator
}  // namespace esphome
