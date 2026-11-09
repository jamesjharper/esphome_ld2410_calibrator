#pragma once
#include "esphome/components/button/button.h"

namespace esphome {
namespace ld2410_calibrator {

class LD2410CalibratorButton : public button::Button {};

#define DEFINE_LD2410_CALIBRATOR_BUTTON(CLASS_NAME, ACTION, PARENT_TYPE) \
  class CLASS_NAME : public ld2410_calibrator::LD2410CalibratorButton, public Parented<PARENT_TYPE> { \
   public: \
    void press_action() override { this->parent_->ACTION; } \
  };

}
}