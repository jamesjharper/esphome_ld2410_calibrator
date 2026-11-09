#include "ld2410_operation_queue.h"
#include <algorithm>

namespace esphome {
namespace ld2410_calibrator {

void LD2410OperationQueue::queue_operation(LD2410OperationType type, uint8_t gate) {
  LD2410Operation operation(type, gate);
  
  // Remove any existing duplicate operation before adding the new one
  remove_duplicate_operation(operation);
  
  // Add the new operation to the end of the queue
  operation_queue_.push_back(operation);
}

optional<LD2410Operation> LD2410OperationQueue::pop() {
  if (operation_queue_.empty()) {
    return {};
  }
  
  LD2410Operation operation = operation_queue_.front();
  operation_queue_.erase(operation_queue_.begin());  // Remove first element
  
  return {operation};
}

void LD2410OperationQueue::remove_duplicate_operation(const LD2410Operation& operation) {
  // Use std::find to locate the duplicate operation
  auto it = std::find(operation_queue_.begin(), operation_queue_.end(), operation);
  
  if (it != operation_queue_.end()) {
    // Found a duplicate, remove it efficiently
    operation_queue_.erase(it);
  }
}

}  // namespace ld2410_calibrator
}  // namespace esphome
