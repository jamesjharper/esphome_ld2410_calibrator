import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

DEPENDENCIES = ["ld2410_calibrator"]

from .. import ld2410_calibrator_ns, LD2410CalibratorComponent

# Configuration constants
CONF_LD2410_CALIBRATOR_ID = "ld2410_calibrator_id"

# Calibration control buttons
CONF_NOISE_FLOOR_START = "noise_floor_start"
CONF_NOISE_FLOOR_STOP = "noise_floor_stop"
CONF_MOVEMENT_START = "movement_start"
CONF_MOVEMENT_STOP = "movement_stop"
CONF_STILL_POSITION_START = "still_position_start"
CONF_STILL_POSITION_STOP = "still_position_stop"
CONF_BLOCKING_POSITION_START = "blocking_position_start"
CONF_BLOCKING_POSITION_STOP = "blocking_position_stop"
CONF_BLOCKING_MOVEMENT_START = "blocking_movement_start"
CONF_BLOCKING_MOVEMENT_STOP = "blocking_movement_stop"
CONF_APPLY_CALIBRATION = "apply_calibration"

# Reset buttons
CONF_RESET_NOISE_FLOOR = "reset_noise_floor"
CONF_RESET_STILL_SIGNAL = "reset_still_signal"
CONF_RESET_MOVEMENT_SIGNAL = "reset_movement_signal"
CONF_RESET_BLOCKING_SIGNAL = "reset_blocking_signal"
CONF_RESET_BLOCKING_MOVEMENT_SIGNAL = "reset_blocking_movement_signal"
CONF_RESET_ALL = "reset_all"

# Button classes
NoiseFloorStartButton = ld2410_calibrator_ns.class_("NoiseFloorStartButton", button.Button)
NoiseFloorStopButton = ld2410_calibrator_ns.class_("NoiseFloorStopButton", button.Button)
MovementStartButton = ld2410_calibrator_ns.class_("MovementStartButton", button.Button)
MovementStopButton = ld2410_calibrator_ns.class_("MovementStopButton", button.Button)
StillPositionStartButton = ld2410_calibrator_ns.class_("StillPositionStartButton", button.Button)
StillPositionStopButton = ld2410_calibrator_ns.class_("StillPositionStopButton", button.Button)
BlockingPositionStartButton = ld2410_calibrator_ns.class_("BlockingPositionStartButton", button.Button)
BlockingPositionStopButton = ld2410_calibrator_ns.class_("BlockingPositionStopButton", button.Button)
BlockingMovementStartButton = ld2410_calibrator_ns.class_("BlockingMovementStartButton", button.Button)
BlockingMovementStopButton = ld2410_calibrator_ns.class_("BlockingMovementStopButton", button.Button)
ApplyCalibrationButton = ld2410_calibrator_ns.class_("ApplyCalibrationButton", button.Button)

ResetNoiseFloorButton = ld2410_calibrator_ns.class_("ResetNoiseFloorButton", button.Button)
ResetStillSignalButton = ld2410_calibrator_ns.class_("ResetStillSignalButton", button.Button)
ResetMovementSignalButton = ld2410_calibrator_ns.class_("ResetMovementSignalButton", button.Button)
ResetBlockingSignalButton = ld2410_calibrator_ns.class_("ResetBlockingSignalButton", button.Button)
ResetBlockingMovementSignalButton = ld2410_calibrator_ns.class_("ResetBlockingMovementSignalButton", button.Button)
ResetAllButton = ld2410_calibrator_ns.class_("ResetAllButton", button.Button)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_LD2410_CALIBRATOR_ID): cv.use_id(LD2410CalibratorComponent),
    
    # Calibration control buttons
    cv.Optional(CONF_NOISE_FLOOR_START): button.button_schema(
        NoiseFloorStartButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:play",
    ),
    cv.Optional(CONF_NOISE_FLOOR_STOP): button.button_schema(
        NoiseFloorStopButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:stop",
    ),
    cv.Optional(CONF_MOVEMENT_START): button.button_schema(
        MovementStartButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:play",
    ),
    cv.Optional(CONF_MOVEMENT_STOP): button.button_schema(
        MovementStopButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:stop",
    ),
    cv.Optional(CONF_STILL_POSITION_START): button.button_schema(
        StillPositionStartButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:play",
    ),
    cv.Optional(CONF_STILL_POSITION_STOP): button.button_schema(
        StillPositionStopButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:stop",
    ),
    cv.Optional(CONF_BLOCKING_POSITION_START): button.button_schema(
        BlockingPositionStartButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:play",
    ),
    cv.Optional(CONF_BLOCKING_POSITION_STOP): button.button_schema(
        BlockingPositionStopButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:stop",
    ),
    cv.Optional(CONF_BLOCKING_MOVEMENT_START): button.button_schema(
        BlockingMovementStartButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:play",
    ),
    cv.Optional(CONF_BLOCKING_MOVEMENT_STOP): button.button_schema(
        BlockingMovementStopButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:stop",
    ),
    cv.Optional(CONF_APPLY_CALIBRATION): button.button_schema(
        ApplyCalibrationButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:check",
    ),
    
    # Reset buttons
    cv.Optional(CONF_RESET_NOISE_FLOOR): button.button_schema(
        ResetNoiseFloorButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:restart",
    ),
    cv.Optional(CONF_RESET_STILL_SIGNAL): button.button_schema(
        ResetStillSignalButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:restart",
    ),
    cv.Optional(CONF_RESET_MOVEMENT_SIGNAL): button.button_schema(
        ResetMovementSignalButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:restart",
    ),
    cv.Optional(CONF_RESET_BLOCKING_SIGNAL): button.button_schema(
        ResetBlockingSignalButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:restart",
    ),
    cv.Optional(CONF_RESET_BLOCKING_MOVEMENT_SIGNAL): button.button_schema(
        ResetBlockingMovementSignalButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:restart",
    ),
    cv.Optional(CONF_RESET_ALL): button.button_schema(
        ResetAllButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:restart-alert",
    ),
})


async def to_ld2410_calibrator_button_code(button_config, parent_var):
    button_var = await button.new_button(button_config)
    await cg.register_parented(button_var, parent_var)


async def to_code(config):
    parent_var = await cg.get_variable(config[CONF_LD2410_CALIBRATOR_ID])

    # Calibration control buttons
    if button_config := config.get(CONF_NOISE_FLOOR_START):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_NOISE_FLOOR_STOP):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_MOVEMENT_START):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_MOVEMENT_STOP):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_STILL_POSITION_START):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_STILL_POSITION_STOP):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_BLOCKING_POSITION_START):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_BLOCKING_POSITION_STOP):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_BLOCKING_MOVEMENT_START):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_BLOCKING_MOVEMENT_STOP):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_APPLY_CALIBRATION):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    # Reset buttons
    if button_config := config.get(CONF_RESET_NOISE_FLOOR):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_RESET_STILL_SIGNAL):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_RESET_MOVEMENT_SIGNAL):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_RESET_BLOCKING_SIGNAL):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_RESET_BLOCKING_MOVEMENT_SIGNAL):
        await to_ld2410_calibrator_button_code(button_config, parent_var)

    if button_config := config.get(CONF_RESET_ALL):
        await to_ld2410_calibrator_button_code(button_config, parent_var)
