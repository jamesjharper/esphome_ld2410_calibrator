#pragma once

#include <vector>
#include "esphome/core/optional.h"

namespace esphome {
namespace ld2410_calibrator {

enum LD2410OperationType {
  SET_GATE_THRESHOLD,
  SET_MAX_DISTANCES_TIMEOUT,
  READ_ALL_INFO,
  NONE
};

struct LD2410Operation {
  LD2410OperationType type;
  uint8_t gate;  // Only used for SET_GATE_THRESHOLD
  
  LD2410Operation(LD2410OperationType op_type = NONE, uint8_t gate_num = 0) 
    : type(op_type), gate(gate_num) {}
    
  // Equality operator for comparing operations
  bool operator==(const LD2410Operation& other) const {
    return type == other.type && gate == other.gate;
  }
};

class LD2410OperationQueue {
 public:
  LD2410OperationQueue() = default;
  
  // Queue an operation, removing any existing duplicate
  void queue_operation(LD2410OperationType type, uint8_t gate = 0);
  
  // Pop and return the next operation from the queue
  optional<LD2410Operation> pop();
  
  // Check if queue is empty
  bool is_empty() const { return operation_queue_.empty(); }
  
  // Get queue size
  size_t size() const { return operation_queue_.size(); }
  
  // Clear all operations
  void clear() { operation_queue_.clear(); }
  
 private:
  std::vector<LD2410Operation> operation_queue_;
  
  // Remove duplicate operation from queue
  void remove_duplicate_operation(const LD2410Operation& operation);
};

}  // namespace ld2410_calibrator
}  // namespace esphome
