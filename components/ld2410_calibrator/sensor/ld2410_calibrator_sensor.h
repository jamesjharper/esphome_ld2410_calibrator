#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace ld2410_calibrator {

// Forward declaration
class LD2410CalibratorComponent;

// Calibration Progress Sensor - reports calibration progress as a percentage
class CalibrationProgressSensor : public sensor::Sensor, public Parented<LD2410CalibratorComponent> {
};

}  // namespace ld2410_calibrator
}  // namespace esphome
