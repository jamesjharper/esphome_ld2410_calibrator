#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/ld2410/ld2410.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "calibrations.h"
#include "ld2410_operation_queue.h"
#include <vector>
#include <cmath>
#include <functional>

namespace esphome {
namespace ld2410_calibrator {

using namespace esphome;

enum CalibrationState {
  INIT,
  IDLE,
  NOISE_FLOOR,
  MOVEMENT_ACTIVE,
  STILL_POSITION,
  BLOCKING_POSITION,
  BLOCKING_MOVEMENT
};


class LD2410CalibratorComponent : public PollingComponent, public EntityBase  {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  
  // Component setup
  void set_ld2410_component(ld2410::LD2410Component *ld2410) { ld2410_component_ = ld2410; }
  void set_still_sensitivity(float sensitivity) {
    still_sensitivity_ = sensitivity; 

    if(calibration_state_ != INIT) {
      apply_calibration();
    }
  }

  void set_movement_sensitivity(float sensitivity) {
    movement_sensitivity_ = sensitivity; 

    if(calibration_state_ != INIT) {
      apply_calibration();
    }
  }

  void set_still_blocking_sensitivity(float sensitivity) {
    still_blocking_sensitivity_ = sensitivity; 
    if(calibration_state_ != INIT) {
      apply_calibration();
    }
  }

  void set_movement_blocking_sensitivity(float sensitivity) {
    movement_blocking_sensitivity_ = sensitivity;
    if(calibration_state_ != INIT) {
      apply_calibration();
    }
  }

  void set_max_movement_detection_distance(float distance) {
    max_movement_detection_distance_ = distance;
    if(calibration_state_ != INIT) {
      apply_calibration();
    }
  }

  void set_max_still_detection_distance(float distance) {
    max_still_detection_distance_ = distance;
    if(calibration_state_ != INIT) {
      apply_calibration();
    }
  }

  void set_noise_floor_duration(int duration) { noise_floor_duration_ = duration; }
  
  // Text sensor setup
  void set_calibration_status_text_sensor(text_sensor::TextSensor *sensor) { 
    calibration_status_text_sensor_ = sensor; 
  }
  
  // Gate sensor configuration
  void set_gate_still_sensor(uint8_t gate, sensor::Sensor *sensor);
  void set_gate_move_sensor(uint8_t gate, sensor::Sensor *sensor);
  void set_gate_still_threshold_number(uint8_t gate, number::Number *number);
  void set_gate_move_threshold_number(uint8_t gate, number::Number *number);
  void set_max_move_distance_gate_number(number::Number *number);
  void set_max_still_distance_gate_number(number::Number *number);
  
  void add_on_calibration_complete_callback(std::function<void()> &&callback) {
    this->calibration_complete_callback_.add(std::move(callback));
  }

  void add_on_noise_floor_calibration_complete_callback(std::function<void()> &&callback) {
    this->noise_floor_calibration_complete_callback_.add(std::move(callback));
  }

  void add_on_still_position_calibration_complete_callback(std::function<void()> &&callback) {
    this->still_position_calibration_complete_callback_.add(std::move(callback));
  }

  void add_on_movement_calibration_complete_callback(std::function<void()> &&callback) {
    this->movement_calibration_complete_callback_.add(std::move(callback));
  }

  void add_on_blocking_position_calibration_complete_callback(std::function<void()> &&callback) {
    this->blocking_position_calibration_complete_callback_.add(std::move(callback));
  }

  void add_on_blocking_movement_calibration_complete_callback(std::function<void()> &&callback) {
    this->blocking_movement_calibration_complete_callback_.add(std::move(callback));
  }

  // Calibration control methods
  void start_noise_floor_calibration();
  void stop_noise_floor_calibration();

  void start_movement_calibration();
  void stop_movement_calibration();

  void start_capture_still_position();
  void stop_capture_still_position();

  void start_capture_blocking_position();
  void stop_capture_blocking_position();

  void start_capture_blocking_movement();
  void stop_capture_blocking_movement();

  void apply_calibration(bool force_update = false);
  
  // Reset methods
  void reset_noise_floor();
  void reset_still_signal();
  void reset_movement_signal();
  void reset_blocking_signal();
  void reset_blocking_movement_signal();
  void reset_all();
  
  // Status getters
  std::string get_calibration_status() const;
  CalibrationState get_calibration_state() const { return calibration_state_; }
  float get_still_sensitivity() const { return still_sensitivity_; }
  float get_movement_sensitivity() const { return movement_sensitivity_; }
  float get_still_blocking_sensitivity() const { return still_blocking_sensitivity_; }
  float get_movement_blocking_sensitivity() const { return movement_blocking_sensitivity_; }
  float get_max_movement_detection_distance() const { return max_movement_detection_distance_; }
  float get_max_still_detection_distance() const { return max_still_detection_distance_; }
  int get_noise_floor_duration() const { return noise_floor_duration_; }
  

  // Methods to connect to LD2410 gate sensors
  void connect_gate_sensors();
  
 

 protected:
  void enter_engineering_mode();
  void exit_engineering_mode();
  
  void apply_gate_thresholds(bool force_update = false);

  // LD2410 operation queue access
  LD2410OperationQueue& get_ld2410_queue() { return ld2410_operation_queue_; }
  
  // Helper methods
  int distance_to_gate(float distance_meters) const {
    // Note this is assuming a gate resolution 0.75m
    int gate = static_cast<int>(distance_meters / 0.75f);
    return std::max(0, std::min(gate, num_gates_ - 1));
  }

  void log_calibration_results();

  void update_status();
  void update_status(const char* msg);
  
  // Gate sensor connection method
  void on_gate_energy_update_(uint8_t gate, float value, bool is_still);

  bool has_enough_samples() {
    switch (calibration_state_) {
      case IDLE:
        return false;
      case NOISE_FLOOR:
        return calibration_data_.has_enough_noise_floor_samples();
      case MOVEMENT_ACTIVE:
        return calibration_data_.has_enough_movement_signal_samples();
      case STILL_POSITION:
        return calibration_data_.has_enough_still_signal_samples();
      case BLOCKING_POSITION:
        return calibration_data_.has_enough_blocking_signal_samples();
      case BLOCKING_MOVEMENT:
        return calibration_data_.has_enough_blocking_movement_samples();
      default:
      return false;
    }
  }

  float sample_progress() const {
    switch (calibration_state_) {
      case IDLE:
        return 100.0f;
      case NOISE_FLOOR:
        return calibration_data_.get_noise_floor_progress_percentage();
      case MOVEMENT_ACTIVE:
        return calibration_data_.get_movement_signal_progress_percentage();
      case STILL_POSITION:
        return calibration_data_.get_still_signal_progress_percentage();
      case BLOCKING_POSITION:
        return calibration_data_.get_blocking_signal_progress_percentage();
      case BLOCKING_MOVEMENT:
        return calibration_data_.get_blocking_movement_progress_percentage();
      default:
        return 100.0f;
    }
  }

  size_t sample_count() const {
    switch (calibration_state_) {
      case IDLE:
        return 0;
      case NOISE_FLOOR:
        return calibration_data_.get_noise_floor_sample_count();
      case MOVEMENT_ACTIVE:
        return calibration_data_.get_movement_signal_sample_count();
      case STILL_POSITION:
        return calibration_data_.get_still_signal_sample_count();
      case BLOCKING_POSITION:
        return calibration_data_.get_blocking_signal_sample_count();
      case BLOCKING_MOVEMENT:
        return calibration_data_.get_blocking_movement_sample_count();
      default:
        return 0;
    }
  }

  uint32_t ms_until_timeout() const {
    uint32_t now = millis();
    return has_sample_acquisition_timed_out(now) ? 0 : (calibration_deadline_ - now);
  }

  bool has_sample_acquisition_timed_out(uint32_t now) const {
    return now > calibration_deadline_;
  }

  void set_calibration_state(CalibrationState new_state) {
    if(new_state == calibration_state_)
      return;


    calibration_state_ = new_state;
    if(calibration_state_ == IDLE) {
      calibration_state_ = IDLE;
      calibration_start_time_ = 0;
      calibration_has_sample_requirement = false;
      update_status();
      return;
    }

    calibration_start_time_ = millis();

    switch (calibration_state_) {
        case NOISE_FLOOR:        
          calibration_deadline_ = calibration_start_time_ + noise_floor_timeout_;
          calibration_has_sample_requirement = !calibration_data_.has_enough_noise_floor_samples();
          break;
        case MOVEMENT_ACTIVE:
          calibration_deadline_ = calibration_start_time_ + movement_timeout_;
          calibration_has_sample_requirement = !calibration_data_.has_enough_movement_signal_samples();
          break;
        case STILL_POSITION:
          calibration_deadline_ = calibration_start_time_ + still_position_capture_timeout_;
          calibration_has_sample_requirement = true;
          calibration_data_.start_next_still_signal_position();
          break;
        case BLOCKING_POSITION:
          calibration_deadline_ = calibration_start_time_ + blocking_position_capture_timeout_;
          calibration_has_sample_requirement = true;
          calibration_data_.start_next_blocking_position();
          break;
        case BLOCKING_MOVEMENT:
          calibration_deadline_ = calibration_start_time_ + movement_timeout_;
          calibration_has_sample_requirement = !calibration_data_.has_enough_blocking_movement_samples();
          break;
        default:
          break;
      }
      update_status();
  }

  ESPPreferenceObject& get_calibration_data_store() {
    if(!this->calibration_data_store_) {
      this->calibration_data_store_ = {global_preferences->make_preference<CalibrationResultsSer>(this->get_object_id_hash())};
    }
    return (*this->calibration_data_store_);
  } 

  void load_calibration_data_from_store();
  void save_calibration_data_to_store();
  void undo_previous_calibration_measurement();

 private:
  ld2410::LD2410Component *ld2410_component_ = nullptr;
  CalibrationState calibration_state_ = INIT;

  CalibrationData calibration_data_;
  optional<ESPPreferenceObject> calibration_data_store_;

  int num_gates_ = 9;
  
  // Timing variables
  uint32_t calibration_start_time_ = 0;
  uint32_t calibration_deadline_ = 0;
  bool calibration_has_sample_requirement = false;

  uint32_t movement_timeout_ = 60000;  
  uint32_t still_position_capture_timeout_ = 10000; 
  uint32_t noise_floor_timeout_ = 120000;  // 120 seconds (2 minutes) for noise floor
  uint32_t blocking_position_capture_timeout_ = 10000;  
  
  // Engineering mode state
  bool engineering_mode_active_ = false;
  bool has_pending_updates_ = false;

  // Configuration values
  float still_sensitivity_ = 0.8f;
  float movement_sensitivity_ = 0.8f;
  float still_blocking_sensitivity_ = 1.0f;
  float movement_blocking_sensitivity_ = 1.0f;
  float max_movement_detection_distance_ = 6.0f;  // Default to 6m (maximum range)
  float max_still_detection_distance_ = 6.0f;     // Default to 6m (maximum range)
  int noise_floor_duration_ = 30;
  CalibrationResultsSer previous_calibration_;

  sensor::Sensor* gate_still_sensors_[9] = {nullptr};
  sensor::Sensor* gate_move_sensors_[9] = {nullptr};
  number::Number* gate_still_threshold_numbers_[9] = {nullptr};
  number::Number* gate_move_threshold_numbers_[9] = {nullptr};
  number::Number* max_move_distance_gate_number_ = nullptr;
  number::Number* max_still_distance_gate_number_ = nullptr;
  
  CallbackManager<void()> calibration_complete_callback_{};
  CallbackManager<void()> noise_floor_calibration_complete_callback_{};
  CallbackManager<void()> still_position_calibration_complete_callback_{};
  CallbackManager<void()> movement_calibration_complete_callback_{};
  CallbackManager<void()> blocking_position_calibration_complete_callback_{};
  CallbackManager<void()> blocking_movement_calibration_complete_callback_{};


  text_sensor::TextSensor *calibration_status_text_sensor_ = nullptr;

  // LD2410 operation queue
  LD2410OperationQueue ld2410_operation_queue_;
  void run_deferred_operations();

};

}  // namespace ld2410_calibrator
}  // namespace esphome
