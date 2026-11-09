#pragma once

#include <vector>
#include <cmath>
#include <functional>

namespace esphome {
namespace ld2410_calibrator {

  static constexpr size_t LD2410_GATES = 9;   

  class MeasurementTimeSeries {

    float mean_ = 0.0f;
    float min_val_ = NAN;
    float max_val_ = NAN;
  
    uint32_t n_ = 0;
    float m2_ = 0.0; 
  
    public:
    float mean() const { return mean_; }
    size_t sample_count() const { return n_; }
  
    float sigma_plus_1() const { return mean_ + (variance_pop());}
    float sigma_plus_2() const { return mean_ + (2.0f * variance_pop());}
    float sigma_minus_1() const { return mean_ - (variance_pop());}
    float sigma_minus_2() const { return mean_ - (2.0f * variance_pop());}


    float variance_pop()  const { return (n_ > 0) ? (m2_ / n_) : NAN; }
    float stdev_pop()     const { return sqrt(variance_pop()); }


    float variance_samp() const { return (n_ > 1) ? (m2_ / (n_ - 1)) : NAN; }
    float stdev_samp()    const { return sqrt(variance_samp()); }
  
    float min() const { return min_val_; }
    float max() const { return max_val_; }
  
    void add_sample(float x) {
        n_++;
        float delta = x - mean_;
        mean_ += delta / static_cast<float>(n_);
        float delta2 = x - mean_;
        m2_ += delta * delta2;

        if (std::isnan(min_val_) || x < min_val_) 
            min_val_ = x;

        if (std::isnan(max_val_) || x > max_val_) 
            max_val_ = x;
    }

    bool clear() {
        bool has_changes = n_ != 0;
        n_ = 0;
        mean_ = m2_ = 0.0f;
        min_val_ = max_val_ = NAN;
        return has_changes;
    }
  };
  
  struct GateCalibrationResultsSer {    

    // Noise
    float still_noise_floor_max_ = 0.0f;
    float still_noise_floor_min_ = 0.0f;
  
    float movement_noise_floor_max_ = 0.0f;
    float movement_noise_floor_min_ = 0.0f;
   
    // Still
    float still_signal_ = NAN;
    float still_signal_floor_ = NAN;
  
    // Movement
    float movement_signal_ = NAN;
    float movement_signal_floor_ = NAN;

    // blocking
    float blocking_signal_ = NAN;
    float blocking_signal_floor_ =NAN;
    
    // Blocking movement
    float blocking_movement_still_signal_ = NAN;
    float blocking_movement_still_signal_floor_ = NAN;

    float blocking_movement_signal_ = NAN;
    float blocking_movement_signal_floor = NAN;
    
  };

  class GateThresholdCalibration {
    GateCalibrationResultsSer r_;

    const size_t min_sample_count_ = 3;
    float still_sensitivity_ = 0.2f;
    float movement_sensitivity_ = 0.2f;
    float still_blocking_sensitivity_ = 1.0f;
    float movement_blocking_sensitivity_ = 1.0f;

    float still_signal_threshold_ = 0.05f;
    float movement_signal_threshold_ = 0.05f;
    float min_still_signal_ceiling_ = 0.15f;
    float min_movement_signal_ceiling_ = 0.15f;

    public:
  
    GateCalibrationResultsSer export_results() {
      return this->r_;
    }

    void import_results(const GateCalibrationResultsSer* r) {
        if(r) {
            this->r_ = *r;
        }
    }  

    bool reset() {
      return reset_noise_floor() | reset_still_signal() | reset_movement_signal() | reset_blocking_signal() | reset_blocking_movement_signal();
    }
  
    bool reset_noise_floor() {
        if(std::isnan(r_.still_noise_floor_max_) && 
           std::isnan(r_.still_noise_floor_min_) && 
           std::isnan(r_.movement_noise_floor_max_) &&
           std::isnan(r_.movement_noise_floor_min_)) {
            return false;
        }
        r_.still_noise_floor_max_ = NAN;
        r_.still_noise_floor_min_ = NAN;
        r_.movement_noise_floor_max_ = NAN;
        r_.movement_noise_floor_min_ = NAN;
        return true;
    }
  
    bool reset_still_signal() {
        if(std::isnan(r_.still_signal_) && std::isnan(r_.still_signal_floor_)) {
            return false;
        }
        r_.still_signal_ = NAN;
        r_.still_signal_floor_ = NAN;
        return true;
    }
  
    bool reset_movement_signal() {
        if(std::isnan(r_.movement_signal_) && std::isnan(r_.movement_signal_floor_)) {
            return false;
        }
        r_.movement_signal_ = NAN;
        r_.movement_signal_floor_ = NAN;
        return true;
    }
  
    bool reset_blocking_signal() {
        if(std::isnan(r_.blocking_signal_) && std::isnan(r_.blocking_signal_floor_)) {
            return false;
        }
        r_.blocking_signal_ = NAN;
        r_.blocking_signal_floor_ = NAN;
        return true;
     }
  
    bool reset_blocking_movement_signal() {
        if(std::isnan(r_.blocking_movement_still_signal_) && std::isnan(r_.blocking_movement_signal_)) {
            return false;
        }
        r_.blocking_movement_still_signal_ = NAN;
        r_.blocking_movement_still_signal_floor_ = NAN;
        r_.blocking_movement_signal_ = NAN;
        r_.blocking_movement_signal_floor = NAN;
        return true;
    }
  
    void set_min_still_signal_ceiling(float min_still_signal_ceiling) {
        min_still_signal_ceiling_ = min_still_signal_ceiling;
    }
  
    void set_min_movement_signal_ceiling(float min_movement_signal_ceiling) {
        min_movement_signal_ceiling_ = min_movement_signal_ceiling;
    }
  
    void set_still_sensitivity(float sensitivity) {
        still_sensitivity_ = sensitivity;
    }

    void set_movement_sensitivity(float sensitivity) {
        movement_sensitivity_ = sensitivity;
    }

    void set_still_blocking_sensitivity(float sensitivity) {
        still_blocking_sensitivity_ = sensitivity;
    }

    void set_movement_blocking_sensitivity(float sensitivity) {
        movement_blocking_sensitivity_ = sensitivity;
    }
  
    void append_still_noise_floor(const MeasurementTimeSeries &ts) {

      // Ignore if there are too few samples
      if(ts.sample_count() <= min_sample_count_) {
        return;
      }

      if(std::isnan(r_.still_noise_floor_max_) || r_.still_noise_floor_max_ < ts.max()) {
        r_.still_noise_floor_max_ = ts.max();
        
      } else {
        // if there are subsequent calibrations, we dont want
        // to ratchet up the max value, so we take the mid point between the previous and the next
        // if the new max value is lower then the previous  
        r_.still_noise_floor_max_ = (ts.max() + r_.still_noise_floor_max_) / 2.0f;
      }
  
      if(std::isnan(r_.still_noise_floor_min_) || r_.still_noise_floor_min_ > ts.min()) {
        r_.still_noise_floor_min_ = ts.min();
      } else {
        r_.still_noise_floor_min_ = (ts.min() + r_.still_noise_floor_min_) / 2.0f;
      }
    }
  
    void append_movement_noise_floor(const MeasurementTimeSeries &ts) {  
       // Ignore if there are too few samples
      if(ts.sample_count() <= min_sample_count_) {
        return;
      }

      if(std::isnan(r_.movement_noise_floor_max_) || r_.movement_noise_floor_max_ < ts.max()) {
        r_.movement_noise_floor_max_ = ts.max();
      } else {
        // if there are subsequent calibrations, we dont want
        // to ratchet up the max value, so we take the mid point between the previous and the next
        // if the new max value is lower then the previous  
        r_.movement_noise_floor_max_ = (ts.max() + r_.movement_noise_floor_max_) / 2.0f;
      }
  
      if(std::isnan(r_.movement_noise_floor_min_) || r_.movement_noise_floor_min_ > ts.min()) {
        r_.movement_noise_floor_min_ = ts.min();
      } else {
        r_.movement_noise_floor_min_ = (ts.min() + r_.movement_noise_floor_min_) / 2.0f;
      }
    }
  
    void append_still_signal(const MeasurementTimeSeries &ts) {
      // Ignore if there are too few samples
      if(ts.sample_count() <= min_sample_count_) {
        return;
      }

      // There will be multiple measurement taken for different points in the room
      // therefore its likely the gates will only be saturated in some measurement
      // while the rest will be noise. 
      // By taking the largest seen value we filter only for measurements where the gate 
      // was saturated. 
      //
      // Movement can result in higher still measurements.
      // By using the mean we help filter for any abnormally high measurements resulting
      if(std::isnan(r_.still_signal_)) {
        r_.still_signal_ = ts.mean();
        r_.still_signal_floor_ = ts.sigma_minus_1();
      } else {
  
        if (r_.still_signal_ < ts.mean()) {
            r_.still_signal_ = ts.mean();
            r_.still_signal_floor_ = ts.sigma_minus_1();
        }
      }
    }
  
    void append_movement_signal(const MeasurementTimeSeries &ts) {
      // Ignore if there are too few samples
      if(ts.sample_count() <= min_sample_count_) {
        return;
      }

      // This time series will contain many samples where the user is moving about
      // This will mean gate energies wont be saturated with signal over the whole time series
      // For this reason we want to take the max seen value for each gate, inorder to capture
      // the moment in it time the gate was saturated
      if(std::isnan(r_.movement_signal_)) {
        r_.movement_signal_ = ts.max();
        r_.movement_signal_floor_ = ts.sigma_minus_1();
      } else {
        if (r_.movement_signal_ < ts.max()) {
            r_.movement_signal_ = ts.max();
            r_.movement_signal_floor_ = ts.sigma_minus_1();
        }
      }
    }
  
    void append_blocking_signal(const MeasurementTimeSeries &ts) {
      // Ignore if there are too few samples
      if(ts.sample_count() <= min_sample_count_) {
        return;
      }

      // There will be multiple measurement taken for different points in the room
      // there fore its likely the gates will only be saturated in some measurement
      // while the reset will likely be noise. 
      // By taking the max seen value we filter only for measurements where the gate 
      // was saturated. 
      //
      // Movement can result in higher still measurements.
      // By using the sigma +2 we help filter for any abnormally high measurements resulting
  
      if(std::isnan(r_.blocking_signal_)) {
        r_.blocking_signal_ = ts.mean();
        r_.blocking_signal_floor_ = ts.sigma_minus_1();
      } else {
        if (r_.blocking_signal_ < ts.mean()) {
            r_.blocking_signal_ = ts.mean();
            r_.blocking_signal_floor_ = ts.sigma_minus_1();
        }
      }
  
    }

    void append_blocking_movement_still_signal(const MeasurementTimeSeries &ts) {
      // Ignore if there are too few samples
      if(ts.sample_count() <= min_sample_count_) {
        return;
      }

      if(std::isnan(r_.blocking_movement_still_signal_)) {
        r_.blocking_movement_still_signal_ = ts.max();
        r_.blocking_movement_still_signal_floor_ = ts.sigma_minus_1();
      } else {
        if (r_.blocking_movement_still_signal_ < ts.max()) {
            r_.blocking_movement_still_signal_ = ts.max();
            r_.blocking_movement_still_signal_floor_ = ts.sigma_minus_1();
        }
      }
    }


    void append_blocking_movement_signal(const MeasurementTimeSeries &ts) {
      // Ignore if there are too few samples
      if(ts.sample_count() <= min_sample_count_) {
        return;
      }

      if(std::isnan(r_.blocking_movement_signal_)) {
        r_.blocking_movement_signal_ = ts.max();
        r_.blocking_movement_signal_floor = ts.sigma_minus_1();
      } else {
        if (r_.blocking_movement_signal_ < ts.max()) {
            r_.blocking_movement_signal_ = ts.max();
            r_.blocking_movement_signal_floor = ts.sigma_minus_1();
        }
      }
    }
  
    float is_sample_still_noise(float sample) {
        if(std::isnan(r_.still_noise_floor_max_)) {
          return false;
        }

        auto signal = sample - r_.still_noise_floor_max_;
        return signal <= still_signal_threshold_;
    }

    float is_sample_movement_noise(float sample) {
        if(std::isnan(r_.movement_noise_floor_max_)) {
          return false;
        }

        auto signal = sample - r_.movement_noise_floor_max_;
        return signal <= still_signal_threshold_;
    }

    bool can_gate_detect_still_signal() {
        auto noise_floor = lower_limit_still_threshold();
        auto signal = upper_limit_still_threshold() - noise_floor;
        return signal <= movement_signal_threshold_;
    }

    bool can_gate_detect_movement_signal() {
        auto noise_floor = lower_limit_movement_threshold();
        auto signal = upper_limit_movement_threshold() - noise_floor;
        return signal <= movement_signal_threshold_;
    }



    float lower_limit_still_threshold() { 

        auto blocking_signal = std::isnan(r_.blocking_signal_)? 0.0f : r_.blocking_signal_;
        auto still_noise_floor_max = std::isnan(r_.still_noise_floor_max_)? 0.0f : r_.still_noise_floor_max_;
       
        float blocking_threshold = still_noise_floor_max + ((blocking_signal - still_noise_floor_max) * still_blocking_sensitivity_);
        return std::max(still_noise_floor_max, blocking_threshold);
    }
  
    float lower_limit_movement_threshold() { 
        auto movement_noise_floor_max = std::isnan(r_.movement_noise_floor_max_)? 0.0f : r_.movement_noise_floor_max_;
        float blocking_movement_threshold = movement_noise_floor_max + ((r_.blocking_movement_signal_ - movement_noise_floor_max) * movement_blocking_sensitivity_);
        return std::max(movement_noise_floor_max, blocking_movement_threshold);
    }
  
    float upper_limit_still_threshold() { 
      return std::isnan(r_.still_signal_)? 1.0f : r_.still_signal_;
    }
  
    float upper_limit_movement_threshold() { 
      return std::isnan(r_.movement_signal_)? 1.0f : r_.movement_signal_;
    }
  
    float get_still_gate_threshold() {
      auto noise_floor = lower_limit_still_threshold();
      auto signal = upper_limit_still_threshold() - noise_floor;
  
      // If the signal is equal to the noise floor, then assume there is no signal for this gate
      // "disable" by setting gate threshold to 100%
      // Note that this doesn't include blocking movement still signal as this is a transient signal 
      // which can be filtered using delays
      if (signal <= still_signal_threshold_) {
        return 1.0f;
      }

      // by using a minimum signal ceiling we prevent gates getting suck in a false positive state or oscillating between states. 
      // Signal to noise will typically by low if the gate was never trigger durring, resulting in a signal close or equal to the noise floor
      if (signal <= min_still_signal_ceiling_) {
        return std::min(1.0f, noise_floor + min_still_signal_ceiling_);
      }

      // Movement can also influence the still signal.
      auto signal_lower_limit = std::max(noise_floor, std::isnan(r_.blocking_movement_still_signal_)? 0.0f : r_.blocking_movement_still_signal_);
      auto signal_amplitude = upper_limit_still_threshold() - signal_lower_limit;
      
      return std::min(1.0f, signal_lower_limit + (signal_amplitude * (1.0f - still_sensitivity_)));
    }
  
    float get_movement_gate_threshold() {
      auto noise_floor = lower_limit_movement_threshold();
      auto signal = upper_limit_movement_threshold() - noise_floor;

      // If the signal is equal to the noise floor, then assume there is no signal for this gate
      // "disable" by setting gate threshold to 100%
      if (signal <= movement_signal_threshold_) {
        return 1.0f;
      }

      // by using a minimum signal ceiling we prevent gates getting suck in a false positive state or oscillating between states. 
      // Signal to noise will typically by low if the gate was never trigger durring, resulting in a signal close or equal to the noise floor
      if (signal <= min_movement_signal_ceiling_) {
         return std::min(1.0f, noise_floor + min_movement_signal_ceiling_);
      }
      return std::min(1.0f,noise_floor + (signal * (1.0f - movement_sensitivity_)));
    }
  
    // Noise
  
    float get_still_noise_floor() {
      return r_.still_noise_floor_min_;
    }
  
    float get_still_noise_ceiling() {
      return r_.still_noise_floor_max_;
    }
  
    float get_movement_noise_floor() {
      return r_.movement_noise_floor_min_;
    }
  
    float get_movement_noise_ceiling() {
      return r_.movement_noise_floor_max_;
    }
  
    float get_still_noise_dynamic_range() {
      return r_.still_noise_floor_max_ - r_.still_noise_floor_min_;
    }
  
    float get_movement_noise_dynamic_range() {
      return r_.movement_noise_floor_max_ - r_.movement_noise_floor_min_;
    }

    // Still Signal
    float get_still_signal_dynamic_range() {
      return r_.still_signal_ - r_.still_signal_floor_;
    }

    float get_still_signal_snr() {
        return upper_limit_still_threshold() / lower_limit_still_threshold();
    }
  
    float get_still_signal_ceiling() {
      return r_.still_signal_;
    }
  
    float get_still_signal_floor() {
      return r_.still_signal_floor_;
    }
  
    // Movement
    
    float get_movement_signal_dynamic_range() {
        return r_.movement_signal_ - r_.movement_signal_floor_;
    }

    float get_movement_signal_snr() {
        return upper_limit_movement_threshold() / lower_limit_movement_threshold();
    }
  
    float get_movement_signal_ceiling() {
      return r_.movement_signal_;
    }
  
     float get_movement_signal_floor() {
       return r_.movement_signal_floor_;
     }
  
    // Blocking
  
     float get_blocking_signal_dynamic_range() {
       return r_.blocking_signal_ - r_.blocking_signal_floor_;
     }
  
     float get_blocking_signal_ceiling() {
       return r_.blocking_signal_;
     }
  
     float get_blocking_signal_floor() {
       return r_.blocking_signal_floor_;
     }

    // Movement Blocking

    float get_blocking_movement_still_signal_ceiling() {
       return r_.blocking_movement_still_signal_;
     }
  
     float get_blocking_movement_still_signal_floor() {
       return r_.blocking_movement_still_signal_floor_;
     }

     float get_blocking_movement_signal_ceiling() {
       return r_.blocking_movement_signal_;
     }
  
     float get_blocking_movement_signal_floor() {
       return r_.blocking_movement_signal_floor;
     }

  };
  
  struct CalibrationResultsSer {
    char version_ = 1;
    GateCalibrationResultsSer gate_results[LD2410_GATES];
      
    size_t get_gate_count() {
        return LD2410_GATES;
    }

    const GateCalibrationResultsSer* gate(size_t gate_idx) const {
      return gate_idx < LD2410_GATES ? &gate_results[gate_idx] : nullptr;
    }
    
    bool set_gate(size_t gate_idx, const GateCalibrationResultsSer& result) {
      if (gate_idx >= LD2410_GATES) {
        return false;  // Index out of bounds
      }

      gate_results[gate_idx] = result;
      return true;
    }
  };

  class CalibrationData {
  
    size_t noise_floor_sample_count_ = 0;
    size_t noise_floor_sample_count_requirement = 600;
    std::vector<MeasurementTimeSeries> still_noise_floor_;
    std::vector<MeasurementTimeSeries> movement_noise_floor_;
  
    size_t still_signal_sample_count_ = 0;
    size_t still_signal_sample_count_per_position_requirement = 5;
    std::vector<MeasurementTimeSeries> still_signal_;
  
    size_t movement_signal_sample_count_ = 0;
    size_t movement_signal_sample_count_requirement = 20;
    std::vector<MeasurementTimeSeries> movement_signal_;
  
    size_t blocking_signal_sample_count_ = 0;
    size_t blocking_signal_sample_count_per_position_requirement = 5;
    std::vector<MeasurementTimeSeries> blocking_signal_;
  
    size_t blocking_movement_sample_count_ = 0;
    size_t blocking_movement_sample_count_requirement = 20;
    std::vector<MeasurementTimeSeries> blocking_movement_still_signal_;
    std::vector<MeasurementTimeSeries> blocking_movement_signal_;
  
    public:
  
    std::vector<GateThresholdCalibration> thresholds;

    CalibrationResultsSer export_results() {
        CalibrationResultsSer results;
        
        // Bounds check to prevent buffer overflow
        size_t gates_to_export = std::min(thresholds.size(), LD2410_GATES);
        for (size_t i = 0; i < gates_to_export; i++) {
            results.set_gate(i, thresholds[i].export_results());
        }     
        return results;
    }
  
    void import_results(CalibrationResultsSer& r) {
        if (thresholds.size() < r.get_gate_count()) {
            thresholds.resize(r.get_gate_count());
        }

        for (size_t i = 0; i < r.get_gate_count(); i++) {
            thresholds[i].import_results(r.gate(i));  
        }
    }
  
    float get_still_threshold(size_t gate_idx) {
      if (gate_idx >= thresholds.size()) {
        return 0.0f;
      }
  
      return thresholds[gate_idx].get_still_gate_threshold();
    }
  
    float get_movement_threshold(size_t gate_idx) {
      if (gate_idx >= thresholds.size()) {
        return 0.0f;
      }
  
      return thresholds[gate_idx].get_movement_gate_threshold();
    }
  
    void apply_still_sensitivity(float sensitivity) {
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].set_still_sensitivity(sensitivity);
      }
    }

    void apply_movement_sensitivity(float sensitivity) {
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].set_movement_sensitivity(sensitivity);
      }
    }

    void apply_still_blocking_sensitivity(float sensitivity) {
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].set_still_blocking_sensitivity(sensitivity);
      }
    }

    void apply_movement_blocking_sensitivity(float sensitivity) {
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].set_movement_blocking_sensitivity(sensitivity);
      }
    }
  
    // Noise Floor
    void add_still_noise_floor_sample(size_t gate_idx, float sample) {
      if (gate_idx >= still_noise_floor_.size()) {
        return;
      }
      still_noise_floor_[gate_idx].add_sample(sample);
      noise_floor_sample_count_ = std::max(noise_floor_sample_count_, still_noise_floor_[gate_idx].sample_count());
    }
  
    void add_movement_noise_floor_sample(size_t gate_idx, float sample) {
      if (gate_idx >= movement_noise_floor_.size()) {
        return;
      }
      movement_noise_floor_[gate_idx].add_sample(sample);
      noise_floor_sample_count_ = std::max(noise_floor_sample_count_, movement_noise_floor_[gate_idx].sample_count());
    }
  
    bool has_enough_noise_floor_samples() {
      return noise_floor_sample_count_ >= noise_floor_sample_count_requirement;
    }
  
    float get_noise_floor_progress_percentage() const {
      if (noise_floor_sample_count_requirement == 0) return 100.0f;
      return std::min(100.0f, (static_cast<float>(noise_floor_sample_count_) / noise_floor_sample_count_requirement) * 100.0f);
    }

    size_t get_noise_floor_sample_count() const {
        return noise_floor_sample_count_;
    }
  
    void apply_noise_floor_calibration() {
      if(!has_enough_noise_floor_samples()) {
        return;
      }
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].append_still_noise_floor(still_noise_floor_[idx]);
        thresholds[idx].append_movement_noise_floor(movement_noise_floor_[idx]);
  
        // Clear data for next sampling session
        still_noise_floor_[idx].clear();
        movement_noise_floor_[idx].clear();
      }
  
      noise_floor_sample_count_ = 0;
    }
  
    // Still Signal
    void start_next_still_signal_position() {
      for (int idx = 0; idx < thresholds.size(); idx++) {
        still_signal_[idx].clear();
      }
      still_signal_sample_count_ = 0;
    }
  
    void add_still_signal_sample(size_t gate_idx, float sample) {
        if (gate_idx >= still_signal_.size()) {
            return;
        }

        // reject samples which are noise
        if(thresholds[gate_idx].is_sample_still_noise(sample)) {
            return;
        }
        still_signal_[gate_idx].add_sample(sample);
        still_signal_sample_count_ = std::max(still_signal_sample_count_, still_signal_[gate_idx].sample_count());
    }
  
    bool has_enough_still_signal_samples() {
      if(still_signal_sample_count_ == 0)
        return false;
      return still_signal_sample_count_ >= still_signal_sample_count_per_position_requirement;
    }
  
    float get_still_signal_progress_percentage() const {
  
      if (still_signal_sample_count_per_position_requirement == 0) return 100.0f;
      return std::min(100.0f, (static_cast<float>(still_signal_sample_count_) / still_signal_sample_count_per_position_requirement) * 100.0f);
    }

    size_t get_still_signal_sample_count() const {
        return still_signal_sample_count_;
    }
  
    void apply_still_signal_calibration() {
      if(!has_enough_still_signal_samples()) {
        return;
      }
  
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].append_still_signal(still_signal_[idx]);
        still_signal_[idx].clear();
      }
  
      still_signal_sample_count_ = 0;
    }
  
    // Move Signal
    void add_move_signal_sample(size_t gate_idx, float sample) {
        if (gate_idx >= movement_signal_.size()) {
          return;
        }

        // reject samples which are noise
        if(thresholds[gate_idx].is_sample_movement_noise(sample)) {
            return;
        }

        movement_signal_[gate_idx].add_sample(sample);
        movement_signal_sample_count_ = std::max(movement_signal_sample_count_, movement_signal_[gate_idx].sample_count());
    }
  
    bool has_enough_movement_signal_samples() {
      return movement_signal_sample_count_ >= movement_signal_sample_count_requirement;
    }
  
    float get_movement_signal_progress_percentage() const {
      if (movement_signal_sample_count_requirement == 0) return 100.0f;
      return std::min(100.0f, (static_cast<float>(movement_signal_sample_count_) / movement_signal_sample_count_requirement) * 100.0f);
    }

    size_t get_movement_signal_sample_count() const {
        return movement_signal_sample_count_;
    }
  
    void apply_movement_signal_calibration() {
      if(!has_enough_movement_signal_samples()) {
        return;
      }
  
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].append_movement_signal(movement_signal_[idx]);
        movement_signal_[idx].clear();
      }
  
      movement_signal_sample_count_ = 0;
    }
  
    // Blocking Signal
    void start_next_blocking_position() {
      for (int idx = 0; idx < thresholds.size(); idx++) {
        blocking_signal_[idx].clear();
      }
      blocking_signal_sample_count_ = 0;
    }
  
    void add_blocking_signal_sample(size_t gate_idx, float sample) {
        if (gate_idx >= blocking_signal_.size()) {
            return;
        }

        blocking_signal_[gate_idx].add_sample(sample);
        blocking_signal_sample_count_ = std::max(blocking_signal_sample_count_, blocking_signal_[gate_idx].sample_count());
    }
  
    bool has_enough_blocking_signal_samples() {
      return blocking_signal_sample_count_ >= blocking_signal_sample_count_per_position_requirement;
    }
  
    float get_blocking_signal_progress_percentage() const {
      if (blocking_signal_sample_count_per_position_requirement == 0) return 100.0f;
      return std::min(100.0f, (static_cast<float>(blocking_signal_sample_count_) / blocking_signal_sample_count_per_position_requirement) * 100.0f);
    }

    size_t get_blocking_signal_sample_count() const {
        return blocking_signal_sample_count_;
    }
  
    void apply_blocking_signal_calibration() {
      if(!has_enough_blocking_signal_samples()) {
        return;
      }
  
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].append_blocking_signal(blocking_signal_[idx]);
        blocking_signal_[idx].clear();
      }
  
      blocking_signal_sample_count_ = 0;
    }

    // Blocking Movement Signal
    void add_blocking_movement_still_signal_sample(size_t gate_idx, float sample) {
        if (gate_idx >= blocking_movement_still_signal_.size()) {
            return;
        }

        blocking_movement_still_signal_[gate_idx].add_sample(sample);
        blocking_movement_sample_count_ = std::max(blocking_movement_sample_count_, blocking_movement_still_signal_[gate_idx].sample_count());
    }

    void add_blocking_movement_signal_sample(size_t gate_idx, float sample) {
        if (gate_idx >= blocking_movement_signal_.size()) {
            return;
        }

        blocking_movement_signal_[gate_idx].add_sample(sample);
        blocking_movement_sample_count_ = std::max(blocking_movement_sample_count_, blocking_movement_signal_[gate_idx].sample_count());
    }

    bool has_enough_blocking_movement_samples() {
      return blocking_movement_sample_count_ >= blocking_movement_sample_count_requirement;
    }

    float get_blocking_movement_progress_percentage() const {
      if (blocking_movement_sample_count_requirement == 0) return 100.0f;
      return std::min(100.0f, (static_cast<float>(blocking_movement_sample_count_) / blocking_movement_sample_count_requirement) * 100.0f);
    }

    size_t get_blocking_movement_sample_count() const {
        return blocking_movement_sample_count_;
    }

    void apply_blocking_movement_calibration() {
      if(!has_enough_blocking_movement_samples()) {
        return;
      }
  
      for (int idx = 0; idx < thresholds.size(); idx++) {
        thresholds[idx].append_blocking_movement_still_signal(blocking_movement_still_signal_[idx]);
        thresholds[idx].append_blocking_movement_signal(blocking_movement_signal_[idx]);
        blocking_movement_still_signal_[idx].clear();
        blocking_movement_signal_[idx].clear();
      }
  
      blocking_movement_sample_count_ = 0;
    }
  
    void initialize(int num_gates) {
      still_noise_floor_.resize(num_gates);
      movement_noise_floor_.resize(num_gates);
      still_signal_.resize(num_gates);
      movement_signal_.resize(num_gates);
      blocking_signal_.resize(num_gates);
      blocking_movement_still_signal_.resize(num_gates);
      blocking_movement_signal_.resize(num_gates);
      thresholds.resize(num_gates);
    }  
  
    bool reset_noise_floor() {
        bool has_change = false;
        for (auto &n : thresholds) { 
            has_change |= n.reset_noise_floor();
        }
        for (auto &n : still_noise_floor_) { 
            n.clear();
        }
        for (auto &n : movement_noise_floor_) {
            n.clear();
        }
        noise_floor_sample_count_ = 0;
        return has_change;
    }
  
    bool reset_still_signal() {
        bool has_change = false;

        for (auto &n : thresholds) { 
            has_change |= n.reset_still_signal();
        }
        for (auto &n : still_signal_) {
            n.clear();
        }
        still_signal_sample_count_ = 0;
        return has_change;
    }
  
    bool reset_movement_signal() {
        bool has_change = false;
        for (auto &n : thresholds) {
            has_change |= n.reset_movement_signal();
        }
        for (auto &n : movement_signal_) {
            n.clear();
        }
        movement_signal_sample_count_ = 0;
        return has_change;
    }
  
    bool reset_blocking_signal() {
        bool has_change = false;
        for (auto &n : thresholds) {
            has_change |= n.reset_blocking_signal();
        }

        for (auto &n : blocking_signal_) {
            n.clear();
        }
        blocking_signal_sample_count_ = 0;
        return has_change;
    }

    bool reset_blocking_movement_signal() {
        bool has_change = false;
        for (auto &n : thresholds) {
            has_change |= n.reset_blocking_movement_signal();
        }

        for (auto &n : blocking_movement_still_signal_) {
            n.clear();
        }
        for (auto &n : blocking_movement_signal_) {
            n.clear();
        }
        blocking_movement_sample_count_ = 0;
        return has_change;
    }
  
    bool reset_all() { 
      return reset_noise_floor() | reset_still_signal() | reset_movement_signal() | reset_blocking_signal() | reset_blocking_movement_signal();
    }
  };

}  // namespace ld2410_calibrator
}  // namespace esphome