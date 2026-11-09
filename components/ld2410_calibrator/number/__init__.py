import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    CONF_INITIAL_VALUE,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_RESTORE_VALUE,
    CONF_STEP,
    CONF_MODE
)

DEPENDENCIES = ["ld2410_calibrator"]
from .. import ld2410_calibrator_ns, LD2410CalibratorComponent

# Configuration constants
CONF_LD2410_CALIBRATOR_ID = "ld2410_calibrator_id"

# Configuration keys
CONF_STILL_SENSITIVITY = "still_sensitivity"
CONF_MOVEMENT_SENSITIVITY = "movement_sensitivity"
CONF_STILL_BLOCKING_SENSITIVITY = "still_blocking_sensitivity"
CONF_MOVEMENT_BLOCKING_SENSITIVITY = "movement_blocking_sensitivity"
CONF_MAX_MOVEMENT_DETECTION_DISTANCE = "max_movement_detection_distance"
CONF_MAX_STILL_DETECTION_DISTANCE = "max_still_detection_distance"
CONF_NOISE_FLOOR_DURATION = "noise_floor_duration"

# Number classes
StillSensitivityNumber = ld2410_calibrator_ns.class_("StillSensitivityNumber", number.Number, cg.Component)
MovementSensitivityNumber = ld2410_calibrator_ns.class_("MovementSensitivityNumber", number.Number, cg.Component)
StillBlockingSensitivityNumber = ld2410_calibrator_ns.class_("StillBlockingSensitivityNumber", number.Number, cg.Component)
MovementBlockingSensitivityNumber = ld2410_calibrator_ns.class_("MovementBlockingSensitivityNumber", number.Number, cg.Component)
MaxMovementDetectionDistanceNumber = ld2410_calibrator_ns.class_("MaxMovementDetectionDistanceNumber", number.Number, cg.Component)
MaxStillDetectionDistanceNumber = ld2410_calibrator_ns.class_("MaxStillDetectionDistanceNumber", number.Number, cg.Component)
NoiseFloorDurationNumber = ld2410_calibrator_ns.class_("NoiseFloorDurationNumber", number.Number, cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_LD2410_CALIBRATOR_ID): cv.use_id(LD2410CalibratorComponent),
    
    # Still Sensitivity Number (0.0 to 1.0, percentage as decimal)
    cv.Optional(CONF_STILL_SENSITIVITY): number.number_schema(
        StillSensitivityNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend({
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES),
        cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_INITIAL_VALUE, default=0.8): cv.float_,
    }),
    
    # Movement Sensitivity Number (0.0 to 1.0, percentage as decimal)
    cv.Optional(CONF_MOVEMENT_SENSITIVITY): number.number_schema(
        MovementSensitivityNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend({
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES),
        cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_INITIAL_VALUE, default=0.8): cv.float_,
    }),
    
    # Still Blocking Sensitivity Number (0.0 to 1.0, percentage as decimal)
    cv.Optional(CONF_STILL_BLOCKING_SENSITIVITY): number.number_schema(
        StillBlockingSensitivityNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend({
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES),
        cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_INITIAL_VALUE, default=1.0): cv.float_,
    }),

    cv.Optional(CONF_MOVEMENT_BLOCKING_SENSITIVITY): number.number_schema(
        MovementBlockingSensitivityNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend({
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES),
        cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_INITIAL_VALUE, default=1.0): cv.float_,
    }),
    
    # Max Movement Detection Distance Number (0.0 to 6.0 meters)
    cv.Optional(CONF_MAX_MOVEMENT_DETECTION_DISTANCE): number.number_schema(
        MaxMovementDetectionDistanceNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend({
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES),
        cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=6.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.25): cv.float_,
        cv.Optional(CONF_INITIAL_VALUE, default=6.0): cv.float_,
    }),
    
    # Max Still Detection Distance Number (0.0 to 6.0 meters)
    cv.Optional(CONF_MAX_STILL_DETECTION_DISTANCE): number.number_schema(
        MaxStillDetectionDistanceNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend({
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES),
        cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=6.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.25): cv.float_,
        cv.Optional(CONF_INITIAL_VALUE, default=6.0): cv.float_,
    }),
    
    # Noise Floor Duration Number (in seconds)
    cv.Optional(CONF_NOISE_FLOOR_DURATION): number.number_schema(
        NoiseFloorDurationNumber,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend({
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES),
        cv.Optional(CONF_MIN_VALUE, default=10): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=300): cv.float_,
        cv.Optional(CONF_STEP, default=1): cv.float_,
        cv.Optional(CONF_INITIAL_VALUE, default=30): cv.float_,
    }),
})

async def to_number_code(config, parented):
    var = await number.new_number(
        config, 
        min_value=config[CONF_MIN_VALUE], 
        max_value=config[CONF_MAX_VALUE], 
        step=config[CONF_STEP])

    if initial_value := config.get(CONF_INITIAL_VALUE):
        cg.add(var.set_initial_value(initial_value))

    if restore_value := config.get(CONF_RESTORE_VALUE):
        cg.add(var.set_restore_value(restore_value))
      
    await cg.register_component(var, config)
    await cg.register_parented(var, parented)

async def to_code(config):
    if number_config := config.get(CONF_STILL_SENSITIVITY):
        await to_number_code(number_config, config[CONF_LD2410_CALIBRATOR_ID])

    if number_config := config.get(CONF_MOVEMENT_SENSITIVITY):
        await to_number_code(number_config, config[CONF_LD2410_CALIBRATOR_ID])

    if number_config := config.get(CONF_STILL_BLOCKING_SENSITIVITY):
        await to_number_code(number_config, config[CONF_LD2410_CALIBRATOR_ID])

    if number_config := config.get(CONF_MOVEMENT_BLOCKING_SENSITIVITY):
        await to_number_code(number_config, config[CONF_LD2410_CALIBRATOR_ID])

    if number_config := config.get(CONF_MAX_MOVEMENT_DETECTION_DISTANCE):
        await to_number_code(number_config, config[CONF_LD2410_CALIBRATOR_ID])

    if number_config := config.get(CONF_MAX_STILL_DETECTION_DISTANCE):
        await to_number_code(number_config, config[CONF_LD2410_CALIBRATOR_ID])

    if number_config := config.get(CONF_NOISE_FLOOR_DURATION):
        await to_number_code(number_config, config[CONF_LD2410_CALIBRATOR_ID])
