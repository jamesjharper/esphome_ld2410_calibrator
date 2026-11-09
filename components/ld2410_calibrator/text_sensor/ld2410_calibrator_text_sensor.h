#pragma once
#include "esphome/components/ld2410_calibrator/ld2410_calibrator.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace ld2410_calibrator {
        
class CalibrationStatusTextSensor : public text_sensor::TextSensor, public Component, public Parented<LD2410CalibratorComponent> {
    public:
     void setup() override {
       // Register this text sensor with the parent component
       if (this->parent_ != nullptr) {
         this->parent_->set_calibration_status_text_sensor(this);
       }
    }
};

}  // namespace ld2410_calibrator
}  // namespace esphome
