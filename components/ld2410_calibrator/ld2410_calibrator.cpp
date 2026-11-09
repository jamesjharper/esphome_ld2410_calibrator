#include "ld2410_calibrator.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ld2410_calibrator {

static const char *const TAG = "ld2410_calibrator";

void LD2410CalibratorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LD2410 Calibrator...");
  
  if (ld2410_component_ == nullptr) {
    ESP_LOGE(TAG, "LD2410 component not set");
    mark_failed();
    return;
  }
  
  // Initialize calibration data structures
  calibration_data_.initialize(num_gates_);

  get_calibration_data_store();
  
  calibration_data_.apply_still_sensitivity(still_sensitivity_);
  calibration_data_.apply_movement_sensitivity(movement_sensitivity_);
  calibration_data_.apply_still_blocking_sensitivity(still_blocking_sensitivity_);
  calibration_data_.apply_movement_blocking_sensitivity(movement_blocking_sensitivity_);

  set_calibration_state(IDLE);
  load_calibration_data_from_store();

}

void LD2410CalibratorComponent::update() {

  if(calibration_state_ == IDLE) {
    // Defer actual update to the hardware device as they are slow and will cause the UI
    run_deferred_operations();
    return;
  }

  update_status();

  uint32_t now = millis();
  switch (calibration_state_) {
    case NOISE_FLOOR:
      if (has_sample_acquisition_timed_out(now) || (calibration_has_sample_requirement && has_enough_samples())) {
        this->stop_noise_floor_calibration();
      }
      
      return;
    case MOVEMENT_ACTIVE:
      if (has_sample_acquisition_timed_out(now)) {
        this->stop_movement_calibration();
      }
      return;
    case STILL_POSITION:
      if (has_sample_acquisition_timed_out(now)) {
        this->stop_capture_still_position();
      }
      return;
    case BLOCKING_POSITION:
      if (has_sample_acquisition_timed_out(now)) {
        this->stop_capture_blocking_position();
      }
      return;
    case BLOCKING_MOVEMENT:
      if (has_sample_acquisition_timed_out(now)) {
        this->stop_capture_blocking_movement();
      }
      return;
    default:
      break;
  }
}

void LD2410CalibratorComponent::run_deferred_operations() {
  // Process all pending LD2410 operations in the queue
  while (true) {
    auto operation = ld2410_operation_queue_.pop();
    if (!operation.has_value()) {
      // No more operations in queue
      break;
    }
    
    if (ld2410_component_ == nullptr) {
      continue;
    }

    switch (operation->type) {
      case SET_GATE_THRESHOLD:
        ESP_LOGI(TAG, "Pushing gate %d data to LD2410 device", operation->gate);
        ld2410_component_->set_gate_threshold(operation->gate);
        break;
      case SET_MAX_DISTANCES_TIMEOUT:
        ESP_LOGI(TAG, "Pushing max distance data to LD2410 device");
        ld2410_component_->set_max_distances_timeout();
        break;
      case READ_ALL_INFO:
        ESP_LOGI(TAG, "Requesting setting data from LD2410 device");
        ld2410_component_->read_all_info();
        break;
      case NONE:
        break;
    }
  }
}

void LD2410CalibratorComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2410 Calibrator:");
  ESP_LOGCONFIG(TAG, "  Still Sensitivity: %.1f%%", still_sensitivity_ * 100);
  ESP_LOGCONFIG(TAG, "  Movement Sensitivity: %.1f%%", movement_sensitivity_ * 100);
  ESP_LOGCONFIG(TAG, "  Still Blocking Sensitivity: %.1f%%", still_blocking_sensitivity_ * 100);
  ESP_LOGCONFIG(TAG, "  Movement Blocking Sensitivity: %.1f%%", movement_blocking_sensitivity_ * 100);
  ESP_LOGCONFIG(TAG, "  Noise Floor Duration: %d seconds", noise_floor_duration_);
  ESP_LOGCONFIG(TAG, "  Number of Gates: %d", num_gates_);
}

// Gate sensor configuration methods
void LD2410CalibratorComponent::set_gate_still_sensor(uint8_t gate, sensor::Sensor *sensor) {
  if (gate >= num_gates_) {
    ESP_LOGE(TAG, "Invalid gate number: %d", gate);
    return;
  }
  if(!sensor) {
    return;
  }

  ESP_LOGCONFIG(TAG, "Configured gate %d still sensor", gate);
  gate_still_sensors_[gate] = sensor;

  auto still_handler = [this, gate](float value) {
    this->on_gate_energy_update_(gate, value, /*still*/ true);
  };

  sensor->add_on_state_callback(still_handler);
}

void LD2410CalibratorComponent::set_gate_move_sensor(uint8_t gate, sensor::Sensor *sensor) {
  if (gate >= num_gates_) {
    ESP_LOGE(TAG, "Invalid gate number: %d", gate);
    return;
  }
  if(!sensor) {
    return;
  }

  ESP_LOGCONFIG(TAG, "Configured gate %d move sensor", gate);
  gate_move_sensors_[gate] = sensor;

  auto still_handler = [this, gate](float value) {
    this->on_gate_energy_update_(gate, value, /*still*/ false);
  };

  sensor->add_on_raw_state_callback(still_handler);
}

void LD2410CalibratorComponent::set_gate_still_threshold_number(uint8_t gate, number::Number *number) {
  if (gate < num_gates_) {
    gate_still_threshold_numbers_[gate] = number;
  } else {
    ESP_LOGE(TAG, "Invalid gate number: %d", gate);
  }
}

void LD2410CalibratorComponent::set_gate_move_threshold_number(uint8_t gate, number::Number *number) {
  if (gate < num_gates_) {
    gate_move_threshold_numbers_[gate] = number;
  } else {
    ESP_LOGE(TAG, "Invalid gate number: %d", gate);
  }
}

void LD2410CalibratorComponent::set_max_move_distance_gate_number(number::Number *number) {
  max_move_distance_gate_number_ = number;
}

void LD2410CalibratorComponent::set_max_still_distance_gate_number(number::Number *number) {
  max_still_distance_gate_number_ = number;
}

// noise floor 
void LD2410CalibratorComponent::start_noise_floor_calibration() {
  if (calibration_state_ != IDLE) {
    ESP_LOGW(TAG, "Cannot start noise floor calibration - another calibration is active");
    return;
  }
  
  ESP_LOGI(TAG, "Starting noise floor calibration for %d seconds", noise_floor_duration_);
  previous_calibration_ = calibration_data_.export_results();
  set_calibration_state(NOISE_FLOOR);
  enter_engineering_mode();
}

void LD2410CalibratorComponent::stop_noise_floor_calibration() {
  if (calibration_state_ != NOISE_FLOOR) {
    return;
  }
  
  ESP_LOGI(TAG, "Stopping noise floor calibration");
  
  set_calibration_state(IDLE);

  exit_engineering_mode();
  calibration_data_.apply_noise_floor_calibration();
  apply_calibration();
  
  save_calibration_data_to_store();

  calibration_complete_callback_.call();
  noise_floor_calibration_complete_callback_.call();
}

// movement 

void LD2410CalibratorComponent::start_movement_calibration() {
  if (calibration_state_ != IDLE) {
    ESP_LOGW(TAG, "Cannot start movement calibration - another calibration is active");
    return;
  }
  
  ESP_LOGI(TAG, "Starting movement calibration");
  previous_calibration_ = calibration_data_.export_results();
  set_calibration_state(MOVEMENT_ACTIVE);
  enter_engineering_mode();
}

void LD2410CalibratorComponent::stop_movement_calibration() {
  if (calibration_state_ != MOVEMENT_ACTIVE) {
    ESP_LOGW(TAG, "Movement calibration is not active");
    return;
  }
  
  ESP_LOGI(TAG, "Stopping movement calibration");
  
  set_calibration_state(IDLE);
  exit_engineering_mode();

  // Apply movement signal calibration
  calibration_data_.apply_movement_signal_calibration();
  apply_calibration();

  save_calibration_data_to_store();

  calibration_complete_callback_.call();
  movement_calibration_complete_callback_.call();
}

// still positions

void LD2410CalibratorComponent::start_capture_still_position() {
  if (calibration_state_ != IDLE) {
    ESP_LOGW(TAG, "Cannot capture still position - another calibration is active");
    return;
  }
  
  ESP_LOGI(TAG, "Starting still position capture for 10 seconds");
  
  previous_calibration_ = calibration_data_.export_results();
  set_calibration_state(STILL_POSITION);
  calibration_data_.start_next_still_signal_position();
  enter_engineering_mode();
}

void LD2410CalibratorComponent::stop_capture_still_position() {
  if (calibration_state_ != STILL_POSITION) {
    return;
  }
  
  ESP_LOGI(TAG, "Stopping still position calibration");
  
  set_calibration_state(IDLE);
  exit_engineering_mode();
  
  calibration_data_.apply_still_signal_calibration();
  apply_calibration();
  
  save_calibration_data_to_store();

  calibration_complete_callback_.call();
  still_position_calibration_complete_callback_.call();
}

// Blocking Position

void LD2410CalibratorComponent::start_capture_blocking_position() {
  if (calibration_state_ != IDLE) {
    ESP_LOGW(TAG, "Cannot capture blocking position - another calibration is active");
    return;
  }
  
  ESP_LOGI(TAG, "Starting blocking position capture for 10 seconds");
  
  previous_calibration_ = calibration_data_.export_results();
  set_calibration_state(BLOCKING_POSITION);
  calibration_data_.start_next_blocking_position();
  
  enter_engineering_mode();
}

void LD2410CalibratorComponent::stop_capture_blocking_position() {
  if (calibration_state_ != BLOCKING_POSITION) {
    return;
  }
  
  ESP_LOGI(TAG, "Stopping blocking position calibration");
  
  set_calibration_state(IDLE);
  exit_engineering_mode();
  
  calibration_data_.apply_blocking_signal_calibration();
  apply_calibration();
  save_calibration_data_to_store();

  calibration_complete_callback_.call(); 
  blocking_position_calibration_complete_callback_.call();
}

// Blocking Movement

void LD2410CalibratorComponent::start_capture_blocking_movement() {
  if (calibration_state_ != IDLE) {
    ESP_LOGW(TAG, "Cannot capture blocking movement - another calibration is active");
    return;
  }
  
  ESP_LOGI(TAG, "Starting blocking movement capture");
  
  previous_calibration_ = calibration_data_.export_results();
  set_calibration_state(BLOCKING_MOVEMENT);
  enter_engineering_mode();
}

void LD2410CalibratorComponent::stop_capture_blocking_movement() {
  if (calibration_state_ != BLOCKING_MOVEMENT) {
    return;
  }
  
  ESP_LOGI(TAG, "Stopping blocking movement calibration");
  
  set_calibration_state(IDLE);
  exit_engineering_mode();
  
  calibration_data_.apply_blocking_movement_calibration();
  apply_calibration();
  save_calibration_data_to_store();

  calibration_complete_callback_.call(); 
  blocking_movement_calibration_complete_callback_.call();
}

// apply calibration

void LD2410CalibratorComponent::apply_calibration(bool force_update) {

  // Update sensitivity in calibration data
  calibration_data_.apply_still_sensitivity(still_sensitivity_);
  calibration_data_.apply_movement_sensitivity(movement_sensitivity_);
  calibration_data_.apply_still_blocking_sensitivity(still_blocking_sensitivity_);
  calibration_data_.apply_movement_blocking_sensitivity(movement_blocking_sensitivity_);
  
  // Apply the calculated thresholds to the device
  apply_gate_thresholds();
  
  log_calibration_results();
}

// Reset methods

void LD2410CalibratorComponent::reset_noise_floor() {
  ESP_LOGI(TAG, "Resetting noise floor calibration data");
  calibration_data_.reset_noise_floor();

  apply_gate_thresholds();
  save_calibration_data_to_store();
  update_status("Noise floor calibration data reset");
}

void LD2410CalibratorComponent::reset_still_signal() {
  ESP_LOGI(TAG, "Resetting still signal calibration data");
  
  calibration_data_.reset_still_signal();
  apply_gate_thresholds();
  save_calibration_data_to_store();

  update_status("Signal calibration data reset");
}

void LD2410CalibratorComponent::reset_movement_signal() {
  ESP_LOGI(TAG, "Resetting movement signal calibration data");
  calibration_data_.reset_movement_signal();
  apply_gate_thresholds();
  save_calibration_data_to_store();

  update_status("Movement calibration data reset");
}

void LD2410CalibratorComponent::reset_blocking_signal() {
  ESP_LOGI(TAG, "Resetting blocking signal calibration data");
  calibration_data_.reset_blocking_signal();
  apply_gate_thresholds();
  save_calibration_data_to_store();

  update_status("Block calibration data reset");
}

void LD2410CalibratorComponent::reset_blocking_movement_signal() {
  ESP_LOGI(TAG, "Resetting blocking movement signal calibration data");
  calibration_data_.reset_blocking_movement_signal();
  apply_gate_thresholds();
  save_calibration_data_to_store();

  update_status("Blocking movement calibration data reset");
}

void LD2410CalibratorComponent::reset_all() {
  ESP_LOGI(TAG, "Resetting all calibration data");
  calibration_data_.reset_all();
  apply_gate_thresholds();
  save_calibration_data_to_store();

  update_status("All calibration data reset");
}

void LD2410CalibratorComponent::apply_gate_thresholds(bool force_update) {

  ESP_LOGI(TAG, "Applying thresholds");
  
  bool any_changes_made = false;
  
  // Apply thresholds through the configured number controls
  for (int gate = 0; gate < num_gates_; gate++) {

    // round the threshold values as ld2410 can only take int percentage values
    float still_threshold = std::roundf(calibration_data_.get_still_threshold(gate) * 100.0f);
    float movement_threshold = std::roundf(calibration_data_.get_movement_threshold(gate) * 100.0f);
    
    ESP_LOGD(TAG, "Gate %d: Still=%.0f, Movement=%.0f", gate, still_threshold, movement_threshold);
    
    bool has_change = force_update;
    // Set thresholds through number controls if configured
    if (gate_still_threshold_numbers_[gate] != nullptr && (force_update || gate_still_threshold_numbers_[gate]->state != still_threshold)) {

      has_change |= true;
      gate_still_threshold_numbers_[gate]->publish_state(still_threshold);
    }
    
    if (gate_move_threshold_numbers_[gate] != nullptr && (force_update || gate_move_threshold_numbers_[gate]->state != movement_threshold)) {
      has_change |= true;
      gate_move_threshold_numbers_[gate]->publish_state(movement_threshold);

    }

    // Limit the number updates to only those necessary as 
    // writing to the hardware is relatively slow
    if(has_change) {
      // Queue gate threshold update to hardware
      ld2410_operation_queue_.queue_operation(SET_GATE_THRESHOLD, gate);
      any_changes_made = true;
    }
  }


  // Calculate max distance gates based on calibration data
  int calibrated_max_still_distance_gate = 0;
  for (int gate = num_gates_ - 1; gate >= 0; gate--) {
    if (!calibration_data_.thresholds[gate].can_gate_detect_still_signal()) {
      calibrated_max_still_distance_gate = gate;
      break;
    }
  }
  
  int calibrated_max_move_distance_gate = 0;
  for (int gate = num_gates_ - 1; gate >= 0; gate--) {
    if (!calibration_data_.thresholds[gate].can_gate_detect_movement_signal()) {
      calibrated_max_move_distance_gate = gate;
      break;
    }
  }

  // Convert distance settings to gate numbers and take the minimum
  int distance_based_max_still_gate = distance_to_gate(max_still_detection_distance_);
  int distance_based_max_move_gate = distance_to_gate(max_movement_detection_distance_);
  
  int max_still_distance_gate = std::min(calibrated_max_still_distance_gate, distance_based_max_still_gate);
  int max_move_distance_gate = std::min(calibrated_max_move_distance_gate, distance_based_max_move_gate);

  ESP_LOGD(TAG, "Distance settings: still=%.1fm (gate %d), movement=%.1fm (gate %d)", 
           max_still_detection_distance_, distance_based_max_still_gate,
           max_movement_detection_distance_, distance_based_max_move_gate);

  bool has_change = force_update;

  // Set the values through number controls if configured
  if (max_still_distance_gate_number_ != nullptr && (force_update || static_cast<int>(max_still_distance_gate_number_->state) != max_still_distance_gate)) {
    max_still_distance_gate_number_->publish_state(static_cast<float>(max_still_distance_gate));
    has_change |= true;
    ESP_LOGD(TAG, "Set max still distance gate to %d via number control", max_still_distance_gate);
  }
  
  if (max_move_distance_gate_number_ != nullptr && (force_update || static_cast<int>(max_move_distance_gate_number_->state) != max_move_distance_gate)) {
    max_move_distance_gate_number_->publish_state(static_cast<float>(max_move_distance_gate));
    has_change |= true;
    ESP_LOGD(TAG, "Set max move distance gate to %d via number control", max_move_distance_gate);
  }

  // Limit the number updates to only those necessary as 
  // writing to the hardware is relatively slow
  if(has_change) {
    // Queue max distances timeout update to hardware
    ld2410_operation_queue_.queue_operation(SET_MAX_DISTANCES_TIMEOUT);
    any_changes_made = true;
  }
  
  // If any changes were made, queue a read_all_info operation to verify settings
  if(any_changes_made) {
    ld2410_operation_queue_.queue_operation(READ_ALL_INFO);
  }
}

void LD2410CalibratorComponent::log_calibration_results() {
  ESP_LOGI(TAG, "LD2410 Calibration Results:");
  ESP_LOGI(TAG, "  Still Sensitivity: %.1f%%", still_sensitivity_ * 100);
  ESP_LOGI(TAG, "  Movement Sensitivity: %.1f%%", movement_sensitivity_ * 100);
  ESP_LOGI(TAG, "  Still Blocking Sensitivity: %.1f%%", still_blocking_sensitivity_ * 100);
  ESP_LOGI(TAG, "  Movement Blocking Sensitivity: %.1f%%", movement_blocking_sensitivity_ * 100);

  // Helper lambda to build calibration array string with aligned columns
  auto build_calibration_string = [this](const std::string& label, std::function<float(int)> getter) -> std::string {
    std::string result = label + ": [";
    for (int gate = 0; gate < num_gates_; gate++) {
      if (gate > 0) result += ", ";

      auto val = getter(gate);
      if(!std::isnan(val)) {
        result += str_sprintf("%6.1f%%", val * 100.0f);  // Right-align with 6 characters width
      } else {
        result += str_sprintf("  -   ");
      }

    }
    result += "]";
    return result;
  };

  auto header_string = [this](const std::string& label, std::function<float(int)> getter) -> std::string {
    std::string result = label + "   ";
    for (int gate = 0; gate < num_gates_; gate++) {
      if (gate > 0) result += ", ";
      result += str_sprintf("%6.2fm", getter(gate));
    }
    return result;
  };

  ESP_LOGI(TAG, "");

  ESP_LOGI(TAG, "     %s", header_string("                        ", 
    [this](int gate) { return (gate + 1) * 0.75f; }).c_str());


  ESP_LOGI(TAG, "  Calibration Data:");

  // Still calibration data
  ESP_LOGI(TAG, "    %s", build_calibration_string("Still Signal:            ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_still_signal_ceiling(); }).c_str());
  ESP_LOGI(TAG, "    %s", build_calibration_string("Still Noise:             ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_still_noise_ceiling(); }).c_str());
  ESP_LOGI(TAG, "    %s", build_calibration_string("Still Blocking:          ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_blocking_signal_ceiling(); }).c_str());
  ESP_LOGI(TAG, "    %s", build_calibration_string("Still Blocking Movement: ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_blocking_movement_still_signal_ceiling(); }).c_str());
  
  // Movement calibration data  
  ESP_LOGI(TAG, "    %s", build_calibration_string("Movement Signal:         ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_movement_signal_ceiling(); }).c_str());
  ESP_LOGI(TAG, "    %s", build_calibration_string("Movement Noise:          ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_movement_noise_ceiling(); }).c_str());
  ESP_LOGI(TAG, "    %s", build_calibration_string("Movement Blocking:       ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_blocking_movement_signal_ceiling(); }).c_str());

  ESP_LOGI(TAG, "  Still Gates:");
  ESP_LOGI(TAG, "    %s", build_calibration_string("Threshold:               ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_still_gate_threshold(); }).c_str());

  ESP_LOGI(TAG, "  Movement Gates:");
  ESP_LOGI(TAG, "    %s", build_calibration_string("Threshold:               ", 
    [this](int gate) { return calibration_data_.thresholds[gate].get_movement_gate_threshold(); }).c_str());
}

// engineering mode

void LD2410CalibratorComponent::enter_engineering_mode() {
  if (engineering_mode_active_ || ld2410_component_ == nullptr) {
    return;
  }
  
  ESP_LOGD(TAG, "Entering engineering mode");
  
  // Use the LD2410Component's method to enable engineering mode
  ld2410_component_->set_engineering_mode(true);
  
  engineering_mode_active_ = true;
  ESP_LOGI(TAG, "Successfully entered engineering mode");
}

void LD2410CalibratorComponent::exit_engineering_mode() {
  if (!engineering_mode_active_ || ld2410_component_ == nullptr) {
    return;
  }
  
  ESP_LOGD(TAG, "Exiting engineering mode");
  
  // Use the LD2410Component's method to disable engineering mode
  ld2410_component_->set_engineering_mode(false);
  
  engineering_mode_active_ = false;
  ESP_LOGI(TAG, "Successfully exited engineering mode");
}

void LD2410CalibratorComponent::update_status() {
  std::string status = get_calibration_status();
  update_status(status.c_str());
}

void LD2410CalibratorComponent::update_status(const char* msg) {

  ESP_LOGI(TAG, "%s", msg);
  
  // Update text sensor if configured
  if (calibration_status_text_sensor_ != nullptr) {
    calibration_status_text_sensor_->publish_state(msg);
  }
}


std::string LD2410CalibratorComponent::get_calibration_status() const {
  uint32_t now = millis();

  switch (calibration_state_) {
    case IDLE:
      return "Idle";
    case NOISE_FLOOR: {

      
      if(calibration_has_sample_requirement) {
        return "Collecting noise floor samples " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(sample_progress())) + "%";
      } else {
        return "Collecting noise floor samples " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(ms_until_timeout() / 1000)) + " seconds remaining";
      }
    }
    case MOVEMENT_ACTIVE: {
      if(calibration_has_sample_requirement) {
        return "Collecting movement data " + std::to_string(sample_count()) + " - " +  std::to_string(static_cast<int>(sample_progress())) + "%";
      } else {
        return "Collecting movement data" + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(ms_until_timeout() / 1000)) + " seconds remaining";
      }

    }
    case STILL_POSITION: {
      if(calibration_has_sample_requirement) {
        return "Collecting still postion data " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(sample_progress())) + "%";
      } else {
        return "Collecting still postion data " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(ms_until_timeout() / 1000)) + " seconds remaining";
      }
    }
    case BLOCKING_POSITION: {
      if(calibration_has_sample_requirement) {
        return "Collecting blocking postion data " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(sample_progress())) + "%";
      } else {
        return "Collecting blocking postion data " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(ms_until_timeout() / 1000)) + " seconds remaining";
      }
    }
    case BLOCKING_MOVEMENT: {
      if(calibration_has_sample_requirement) {
        return "Collecting blocking movement data " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(sample_progress())) + "%";
      } else {
        return "Collecting blocking movement data " + std::to_string(sample_count()) + " - " + std::to_string(static_cast<int>(ms_until_timeout() / 1000)) + " seconds remaining";
      }
    }
    default:
      return "Unknown state";
  }
}

void LD2410CalibratorComponent::on_gate_energy_update_(uint8_t gate, float value, bool is_still) {
  if (gate >= num_gates_) {
    return;
  }
  
  // Collect sample based on current calibration state
  switch (calibration_state_) {
    case IDLE:
      return;
    case NOISE_FLOOR:
      if (is_still) {
        calibration_data_.add_still_noise_floor_sample(gate, value / 100.0f);
      } else {
        calibration_data_.add_movement_noise_floor_sample(gate, value / 100.0f);
      }
      break;
      
    case MOVEMENT_ACTIVE:
      if (!is_still) {
        calibration_data_.add_move_signal_sample(gate, value / 100.0f);   
        calibration_has_sample_requirement = !calibration_data_.has_enough_noise_floor_samples();
      }
      break;
      
    case STILL_POSITION:
      if (is_still) {  
        calibration_data_.add_still_signal_sample(gate, value / 100.0f);
        calibration_has_sample_requirement = !calibration_data_.has_enough_still_signal_samples();
      }
      break;
      
    case BLOCKING_POSITION:
      if (is_still) { 
        calibration_data_.add_blocking_signal_sample(gate, value / 100.0f);
        calibration_has_sample_requirement = !calibration_data_.has_enough_blocking_signal_samples();
      }
      break;
      
    case BLOCKING_MOVEMENT:
      if (is_still) { 
        calibration_data_.add_blocking_movement_still_signal_sample(gate, value / 100.0f);
      } else {
        calibration_data_.add_blocking_movement_signal_sample(gate, value / 100.0f);
      }
      calibration_has_sample_requirement = !calibration_data_.has_enough_blocking_movement_samples();
      break;
      
    default:
      break;
  }
}

// Calibration data persistence methods

void LD2410CalibratorComponent::load_calibration_data_from_store() {
  CalibrationResultsSer loaded_calibrations; 
  ESP_LOGI(TAG, "Loading calibration data from non-volatile storage...");

  if(this->get_calibration_data_store().load(&loaded_calibrations)) {
    update_status("Calibration data loaded");
    calibration_data_.import_results(loaded_calibrations);
  
    apply_calibration();
  } else {
    update_status("No calibration data");
  }
}

void LD2410CalibratorComponent::save_calibration_data_to_store() {
  
  ESP_LOGI(TAG, "Saving calibration data to non-volatile storage...");

  CalibrationResultsSer results_to_save = calibration_data_.export_results();

  if(this->get_calibration_data_store().save(&results_to_save)) {
    ESP_LOGI(TAG, "Successfully saved calibration datas");
  } else {
    ESP_LOGE(TAG, "Failed to save calibration data to storage");
  }
}

void LD2410CalibratorComponent::undo_previous_calibration_measurement() {
  
  ESP_LOGI(TAG, "Reverting to previous calibration state...");

  calibration_data_.import_results(previous_calibration_);
  apply_calibration();
}


}  // namespace ld2410_calibrator
}  // namespace esphome
