<<<<<<< HEAD
# esphome_ld2410_calibrator
calibration helper which replicates and improves on the Hi-link's calibration app for  ld2410 and like devices
=======
# LD2410 Calibrator - ESPHome Custom Component

This ESPHome custom component provides automated calibration capabilities for the LD2410 mmWave radar sensor. It works in conjunction with the existing ESPHome LD2410 component to optimize detection thresholds based on your specific environment.

## Features

- **Automated Calibration Process**: Systematically calibrates both still and movement detection thresholds
- **Multiple Calibration Modes**: Supports noise floor, presence, movement, and blocking signal calibration
- **Statistical Analysis**: Uses mean, standard deviation, min/max analysis for optimal threshold calculation
- **Home Assistant Integration**: Exposes buttons, numbers, and status sensors for easy control
- **Engineering Mode Support**: Automatically manages LD2410 engineering mode for data collection

## Installation

1. Copy the `esphome_ld2410_calibrator` folder to your ESPHome `custom_components` directory
2. Include the component in your ESPHome configuration as shown in the example below

## Configuration

### Basic Configuration

```yaml
# Required: Main LD2410 component
ld2410:
  id: ld2410_radar

# LD2410 Calibrator
ld2410_calibrator:
  id: calibrator
  ld2410_id: ld2410_radar
  sensitivity: 0.8  # Optional: 80% sensitivity (0.0-1.0), default: 0.8
  noise_floor_duration: 30  # Optional: seconds, default: 30
```

### UI Components

Add these components to expose calibration controls in Home Assistant:

```yaml
# Calibration Buttons
button:
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Calibrate Noise Floor"
    
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Start Movement Calibration"
    
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Stop Movement Calibration"
    
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Capture Still Position"
    
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Capture Blocking Position"
    
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Apply Calibration"

# Configuration Numbers
number:
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Calibration Sensitivity"
    min_value: 0.0
    max_value: 1.0
    step: 0.01
    
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Noise Floor Duration"
    min_value: 5
    max_value: 120
    step: 5

# Status Display
text_sensor:
  - platform: ld2410_calibrator
    ld2410_calibrator_id: calibrator
    name: "Calibration Status"
```

## Calibration Process

Follow these steps in order for optimal results:

### 1. Noise Floor Calibration
- **Purpose**: Establishes baseline energy levels when the room is empty
- **Duration**: Configurable (default: 30 seconds)
- **Instructions**: 
  1. Ensure the room is completely empty
  2. Press "Calibrate Noise Floor" button
  3. Wait for completion (status will update)

### 2. Presence Signal Calibration
- **Purpose**: Captures energy signatures for desired detection positions
- **Duration**: 10 seconds per position
- **Instructions**:
  1. Stand in a position where you want presence to be detected
  2. Press "Capture Still Position" button
  3. Remain completely still for 10 seconds
  4. Repeat for all desired detection positions in the room

### 3. Movement Signal Calibration
- **Purpose**: Captures energy signatures for movement patterns
- **Duration**: Up to 60 seconds (or until manually stopped)
- **Instructions**:
  1. Press "Start Movement Calibration" button
  2. Walk around the room in all the ways you want movement to be detected
  3. Include normal walking patterns, arm movements, etc.
  4. Press "Stop Movement Calibration" when complete (or wait for 60-second timeout)

### 4. Blocking Signal Calibration
- **Purpose**: Captures energy signatures for positions where detection should NOT occur
- **Duration**: 10 seconds per position
- **Instructions**:
  1. Stand in a position where you do NOT want presence to be detected
  2. Press "Capture Blocking Position" button
  3. Remain completely still for 10 seconds
  4. Repeat for all positions where false positives should be avoided

### 5. Apply Calibration
- **Purpose**: Calculates and applies the optimized thresholds
- **Instructions**:
  1. Ensure all previous calibration steps are complete
  2. Press "Apply Calibration" button
  3. The component will calculate optimal thresholds and apply them to the LD2410

## Calibration Algorithms

### Still Gate Thresholds
```
gate_still_threshold[n] = (still_signal[n].mean - Max(noise_floor[n].max, blocking_signal[n].sigma_plus_2)) × sensitivity
```

### Movement Gate Thresholds
```
gate_movement_threshold[n] = (movement_signal[n].mean - noise_floor_movement[n].max) × sensitivity
```

**Note**: If any calculated threshold is negative, it's automatically set to the noise floor value and an error is logged.

## Parameters

### Sensitivity
- **Range**: 0.0 to 1.0 (0% to 100%)
- **Default**: 0.8 (80%)
- **Effect**: 
  - Higher values = more sensitive detection (more likely to detect presence/movement)
  - Lower values = less sensitive detection (fewer false positives)

### Noise Floor Duration
- **Range**: 5 to 120 seconds
- **Default**: 30 seconds
- **Effect**: Longer duration provides more stable baseline measurements

## Status Messages

The calibration status sensor will display:
- `"Ready for calibration"` - Component initialized and ready
- `"Collecting noise floor data..."` - Noise floor calibration in progress
- `"Noise floor calibration complete"` - Noise floor data collected
- `"Movement calibration active - walk around the room"` - Movement calibration in progress
- `"Movement calibration complete"` - Movement data collected
- `"Capturing still position - do not move"` - Still position capture in progress
- `"Still position capture complete"` - Still position data collected
- `"Capturing blocking position - do not move"` - Blocking position capture in progress
- `"Blocking position capture complete"` - Blocking position data collected
- `"Calibration applied successfully"` - Thresholds calculated and applied
- `"Error: Insufficient calibration data"` - Missing required calibration data

## Troubleshooting

### Common Issues

1. **"Insufficient calibration data" error**
   - Ensure all calibration steps (noise floor, presence, movement, blocking) have been completed
   - Check the logs for specific missing data types

2. **Negative threshold warnings**
   - This indicates the calculated threshold would be below the noise floor
   - Consider adjusting sensitivity or recalibrating with different positions
   - The component automatically uses the noise floor as a minimum threshold

3. **No response from buttons**
   - Verify the LD2410 component is properly configured and connected
   - Check UART connections and baud rate settings
   - Ensure the component has successfully initialized (check logs)

### Logging

Enable detailed logging to troubleshoot issues:

```yaml
logger:
  level: DEBUG
  logs:
    ld2410_calibrator: DEBUG
```

## Technical Details

- **Data Collection Rate**: 10 Hz (100ms intervals)
- **Statistical Analysis**: Calculates mean, standard deviation, min/max, ±2σ values
- **Gate Count**: Supports 8 gates (standard LD2410 configuration)
- **Engineering Mode**: Automatically managed during calibration phases

## Dependencies

- ESPHome LD2410 component
- UART communication with LD2410 sensor

## License

This component is provided as-is for educational and personal use. Please test thoroughly before deploying in production environments.
>>>>>>> 1894047 (Initial commit)
