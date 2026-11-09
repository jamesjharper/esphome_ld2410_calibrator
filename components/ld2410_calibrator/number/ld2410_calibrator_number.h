#pragma once
#include "esphome/components/ld2410_calibrator/ld2410_calibrator.h"
#include "esphome/core/preferences.h"
#include "esphome/components/number/number.h"
#include "esphome/core/component.h"

namespace esphome {
namespace ld2410_calibrator {

// Forward declaration
class LD2410CalibratorComponent;

class LD2410CalibratorNumber : public number::Number, public Component {
 protected:
  optional<float> initial_value_{};
  bool restore_value_ = true;
  ESPPreferenceObject pref_;

 public:
  float get_setup_priority() const override { return setup_priority::DATA + 1.0f; }

  optional<float> get_initial_value();

  void set_initial_value(float initial_value) { this->initial_value_ = initial_value; }
  void set_restore_value(bool restore_value) { this->restore_value_ = restore_value; }

  void setup() override;

  void dump_config() override;
  virtual void set_action(float value) = 0;
};

#define DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(CLASS_NAME, SETTER, PARENT_TYPE) \
  class CLASS_NAME : public ld2410_calibrator::LD2410CalibratorNumber, public Parented<PARENT_TYPE> { \
   protected: \
    void control(float value) override { \
      this->publish_state(value); \
      set_action(value); \
      if (this->restore_value_) \
        this->pref_.save(&value); \
    } \
    \
    void set_action(float value) { this->parent_->SETTER; } \
  };

DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(StillSensitivityNumber, set_still_sensitivity(value), LD2410CalibratorComponent)
DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(MovementSensitivityNumber, set_movement_sensitivity(value), LD2410CalibratorComponent)
DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(StillBlockingSensitivityNumber, set_still_blocking_sensitivity(value), LD2410CalibratorComponent)
DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(MovementBlockingSensitivityNumber, set_movement_blocking_sensitivity(value), LD2410CalibratorComponent)
DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(MaxMovementDetectionDistanceNumber, set_max_movement_detection_distance(value), LD2410CalibratorComponent)
DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(MaxStillDetectionDistanceNumber, set_max_still_detection_distance(value), LD2410CalibratorComponent)
DEFINE_LD2410_CALIBRATOR_NUMBER_CONTROL(NoiseFloorDurationNumber, set_noise_floor_duration((int) value), LD2410CalibratorComponent)

}  // namespace ld2410_calibrator
}  // namespace esphome
