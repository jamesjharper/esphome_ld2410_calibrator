
#pragma once


#include "esphome/components/ld2410_calibrator/ld2410_calibrator.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace ld2410_calibrator {

// Action classes - defined here so they have access to the full component definition
template<typename... Ts> class StartNoiseFloorCalibrationAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->start_noise_floor_calibration(); }
   };
   
   template<typename... Ts> class StopNoiseFloorCalibrationAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->stop_noise_floor_calibration(); }
   };
   
   template<typename... Ts> class StartMovementCalibrationAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->start_movement_calibration(); }
   };
   
   template<typename... Ts> class StopMovementCalibrationAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->stop_movement_calibration(); }
   };
   
   template<typename... Ts> class StartStillPositionCaptureAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->start_capture_still_position(); }
   };
   
   template<typename... Ts> class StopStillPositionCaptureAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->stop_capture_still_position(); }
   };
   
   template<typename... Ts> class StartBlockingPositionCaptureAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->start_capture_blocking_position(); }
   };
   
   template<typename... Ts> class StopBlockingPositionCaptureAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->stop_capture_blocking_position(); }
   };
   
   template<typename... Ts> class StartBlockingMovementCaptureAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->start_capture_blocking_movement(); }
   };
   
   template<typename... Ts> class StopBlockingMovementCaptureAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->stop_capture_blocking_movement(); }
   };
   
   template<typename... Ts> class ApplyCalibrationAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->apply_calibration(); }
   };
   
   template<typename... Ts> class ResetAllCalibrationAction : public Action<Ts...>, public Parented<LD2410CalibratorComponent> {
    public:
     void play(Ts... x) override { this->parent_->reset_all(); }
   };
   
  class CalibrationCompleteTrigger : public Trigger<> {
    public:
     explicit CalibrationCompleteTrigger(LD2410CalibratorComponent *parent) {
       parent->add_on_calibration_complete_callback([this]() {
         this->trigger();
       });
     }
  }; 


  class NoiseFloorCalibrationCompleteTrigger : public Trigger<> {
    public:
     explicit NoiseFloorCalibrationCompleteTrigger(LD2410CalibratorComponent *parent) {
       parent->add_on_noise_floor_calibration_complete_callback([this]() {
         this->trigger();
       });
     }
  }; 

  class StillPositionCalibrationCompleteTrigger : public Trigger<> {
    public:
     explicit StillPositionCalibrationCompleteTrigger(LD2410CalibratorComponent *parent) {
       parent->add_on_still_position_calibration_complete_callback([this]() {
         this->trigger();
       });
     }
  }; 

  class MovementCalibrationCompleteTrigger : public Trigger<> {
    public:
     explicit MovementCalibrationCompleteTrigger(LD2410CalibratorComponent *parent) {
       parent->add_on_movement_calibration_complete_callback([this]() {
         this->trigger();
       });
     }
  }; 

  class BlockingPositionCalibrationCompleteTrigger : public Trigger<> {
    public:
     explicit BlockingPositionCalibrationCompleteTrigger(LD2410CalibratorComponent *parent) {
       parent->add_on_blocking_position_calibration_complete_callback([this]() {
         this->trigger();
       });
     }
  }; 

  class BlockingMovementCalibrationCompleteTrigger : public Trigger<> {
    public:
     explicit BlockingMovementCalibrationCompleteTrigger(LD2410CalibratorComponent *parent) {
       parent->add_on_blocking_movement_calibration_complete_callback([this]() {
         this->trigger();
       });
     }
  }; 

}  // namespace ld2410_calibrator
}  // namespace esphome
