import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome.components import ld2410, sensor, number
from esphome import automation

DEPENDENCIES = ["ld2410"]
CODEOWNERS = ["@esphome"]
MULTI_CONF = True

CONF_LD2410_ID = "ld2410_id"
CONF_STILL_SENSITIVITY = "still_sensitivity"
CONF_MOVEMENT_SENSITIVITY = "movement_sensitivity"
CONF_STILL_BLOCKING_SENSITIVITY = "still_blocking_sensitivity"
CONF_MOVEMENT_BLOCKING_SENSITIVITY = "movement_blocking_sensitivity"
CONF_MAX_MOVEMENT_DETECTION_DISTANCE = "max_movement_detection_distance"
CONF_MAX_STILL_DETECTION_DISTANCE = "max_still_detection_distance"
CONF_NOISE_FLOOR_DURATION = "noise_floor_duration"
CONF_GATE_SENSORS = "gate_sensors"
CONF_GATE = "gate"
CONF_STILL_SENSOR = "still_sensor"
CONF_MOVE_SENSOR = "move_sensor"
CONF_STILL_THRESHOLD = "still_threshold"
CONF_MOVE_THRESHOLD = "move_threshold"
CONF_MAX_MOVE_DISTANCE_GATE = "max_move_distance_gate"
CONF_MAX_STILL_DISTANCE_GATE = "max_still_distance_gate"

CONF_ON_CALIBRATE = "on_calibrate_complete"
CONF_ON_NOISE_FLOOR_CALIBRATE = "on_calibrate_noise_floor_complete"
CONF_ON_STILL_POSITION_CALIBRATE = "on_calibrate_still_position_complete"
CONF_ON_MOVEMENT_CALIBRATE = "on_calibrate_movement_complete"
CONF_ON_BLOCKING_POSITION_CALIBRATE = "on_calibrate_blocking_position_complete"

ld2410_calibrator_ns = cg.esphome_ns.namespace("ld2410_calibrator")
LD2410CalibratorComponent = ld2410_calibrator_ns.class_(
    "LD2410CalibratorComponent", cg.Component
)

CalibrationCompleteTrigger = ld2410_calibrator_ns.class_("CalibrationCompleteTrigger", automation.Trigger.template())
NoiseFloorCalibrationCompleteTrigger = ld2410_calibrator_ns.class_("NoiseFloorCalibrationCompleteTrigger", automation.Trigger.template())
StillPositionCalibrationCompleteTrigger = ld2410_calibrator_ns.class_("StillPositionCalibrationCompleteTrigger", automation.Trigger.template())
MovementCalibrationCompleteTrigger = ld2410_calibrator_ns.class_("MovementCalibrationCompleteTrigger", automation.Trigger.template())
BlockingPositionCalibrationCompleteTrigger = ld2410_calibrator_ns.class_("BlockingPositionCalibrationCompleteTrigger", automation.Trigger.template())

GATE_SENSOR_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_GATE): cv.int_range(min=0, max=8),
        cv.Optional(CONF_STILL_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_MOVE_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_STILL_THRESHOLD): cv.use_id(number.Number),
        cv.Optional(CONF_MOVE_THRESHOLD): cv.use_id(number.Number),
    }
)


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LD2410CalibratorComponent),
            cv.GenerateID(CONF_LD2410_ID): cv.use_id(ld2410.LD2410Component),
            cv.Optional(CONF_STILL_SENSITIVITY, default=0.8): cv.float_range(min=0.0, max=1.0),
            cv.Optional(CONF_MOVEMENT_SENSITIVITY, default=0.8): cv.float_range(min=0.0, max=1.0),
            cv.Optional(CONF_STILL_BLOCKING_SENSITIVITY, default=1.0): cv.float_range(min=0.0, max=1.0),
            cv.Optional(CONF_MOVEMENT_BLOCKING_SENSITIVITY, default=1.0): cv.float_range(min=0.0, max=1.0),
            cv.Optional(CONF_MAX_MOVEMENT_DETECTION_DISTANCE, default=6.0): cv.float_range(min=0.0, max=6.0),
            cv.Optional(CONF_MAX_STILL_DETECTION_DISTANCE, default=6.0): cv.float_range(min=0.0, max=6.0),
            cv.Optional(CONF_NOISE_FLOOR_DURATION, default=30): cv.int_range(min=5, max=120),
            cv.Optional(CONF_GATE_SENSORS, default=[]): cv.ensure_list(GATE_SENSOR_SCHEMA),
            cv.Optional(CONF_MAX_MOVE_DISTANCE_GATE): cv.use_id(number.Number),
            cv.Optional(CONF_MAX_STILL_DISTANCE_GATE): cv.use_id(number.Number),
            cv.Optional(CONF_ON_CALIBRATE): automation.validate_automation({
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CalibrationCompleteTrigger),
            }),
            cv.Optional(CONF_ON_NOISE_FLOOR_CALIBRATE): automation.validate_automation({
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(NoiseFloorCalibrationCompleteTrigger),
            }),
            cv.Optional(CONF_ON_STILL_POSITION_CALIBRATE): automation.validate_automation({
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(StillPositionCalibrationCompleteTrigger),
            }),
            cv.Optional(CONF_ON_MOVEMENT_CALIBRATE): automation.validate_automation({
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(MovementCalibrationCompleteTrigger),
            }),
            cv.Optional(CONF_ON_BLOCKING_POSITION_CALIBRATE): automation.validate_automation({
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(BlockingPositionCalibrationCompleteTrigger),
            }),
        }
    )
    .extend(cv.polling_component_schema("1s"))
)

# Actions
StartNoiseFloorCalibrationAction = ld2410_calibrator_ns.class_(

    "StartNoiseFloorCalibrationAction", automation.Action
)
StopNoiseFloorCalibrationAction = ld2410_calibrator_ns.class_(
    "StopNoiseFloorCalibrationAction", automation.Action
)
StartMovementCalibrationAction = ld2410_calibrator_ns.class_(
    "StartMovementCalibrationAction", automation.Action
)
StopMovementCalibrationAction = ld2410_calibrator_ns.class_(
    "StopMovementCalibrationAction", automation.Action
)
StartStillPositionCaptureAction = ld2410_calibrator_ns.class_(
    "StartStillPositionCaptureAction", automation.Action
)
StopStillPositionCaptureAction = ld2410_calibrator_ns.class_(
    "StopStillPositionCaptureAction", automation.Action
)
StartBlockingPositionCaptureAction = ld2410_calibrator_ns.class_(
    "StartBlockingPositionCaptureAction", automation.Action
)
StopBlockingPositionCaptureAction = ld2410_calibrator_ns.class_(
    "StopBlockingPositionCaptureAction", automation.Action
)
StartBlockingMovementCaptureAction = ld2410_calibrator_ns.class_(
    "StartBlockingMovementCaptureAction", automation.Action
)
StopBlockingMovementCaptureAction = ld2410_calibrator_ns.class_(
    "StopBlockingMovementCaptureAction", automation.Action
)
ApplyCalibrationAction = ld2410_calibrator_ns.class_(
    "ApplyCalibrationAction", automation.Action
)
ResetAllCalibrationAction = ld2410_calibrator_ns.class_(
    "ResetAllCalibrationAction", automation.Action
)

# Action schemas
LD2410_CALIBRATOR_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(LD2410CalibratorComponent),
    }
)

@automation.register_action(
    "ld2410_calibrator.start_noise_floor_calibration",
    StartNoiseFloorCalibrationAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.stop_noise_floor_calibration",
    StopNoiseFloorCalibrationAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.start_movement_calibration",
    StartMovementCalibrationAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.stop_movement_calibration",
    StopMovementCalibrationAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.start_still_position_capture",
    StartStillPositionCaptureAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.stop_still_position_capture",
    StopStillPositionCaptureAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.start_blocking_position_capture",
    StartBlockingPositionCaptureAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.stop_blocking_position_capture",
    StopBlockingPositionCaptureAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.start_blocking_movement_capture",
    StartBlockingMovementCaptureAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.stop_blocking_movement_capture",
    StopBlockingMovementCaptureAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.apply_calibration",
    ApplyCalibrationAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
@automation.register_action(
    "ld2410_calibrator.reset_all_calibration",
    ResetAllCalibrationAction,
    LD2410_CALIBRATOR_ACTION_SCHEMA,
)
async def ld2410_calibrator_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    ld2410_component = await cg.get_variable(config[CONF_LD2410_ID])
    cg.add(var.set_ld2410_component(ld2410_component))
    
    # Set sensitivity and noise floor duration
    cg.add(var.set_still_sensitivity(config[CONF_STILL_SENSITIVITY]))
    cg.add(var.set_movement_sensitivity(config[CONF_MOVEMENT_SENSITIVITY]))
    cg.add(var.set_still_blocking_sensitivity(config[CONF_STILL_BLOCKING_SENSITIVITY]))
    cg.add(var.set_movement_blocking_sensitivity(config[CONF_MOVEMENT_BLOCKING_SENSITIVITY]))
    cg.add(var.set_max_movement_detection_distance(config[CONF_MAX_MOVEMENT_DETECTION_DISTANCE]))
    cg.add(var.set_max_still_detection_distance(config[CONF_MAX_STILL_DETECTION_DISTANCE]))
    cg.add(var.set_noise_floor_duration(config[CONF_NOISE_FLOOR_DURATION]))

    for trig_config in config.get(CONF_ON_CALIBRATE, []):
        trigger = cg.new_Pvariable(trig_config[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], trig_config)
    
    for trig_config in config.get(CONF_ON_NOISE_FLOOR_CALIBRATE, []):
        trigger = cg.new_Pvariable(trig_config[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], trig_config)

    for trig_config in config.get(CONF_ON_STILL_POSITION_CALIBRATE, []):
        trigger = cg.new_Pvariable(trig_config[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], trig_config)

    for trig_config in config.get(CONF_ON_MOVEMENT_CALIBRATE, []):
        trigger = cg.new_Pvariable(trig_config[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], trig_config)

    for trig_config in config.get(CONF_ON_BLOCKING_POSITION_CALIBRATE, []):
        trigger = cg.new_Pvariable(trig_config[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], trig_config)

    # Configure gate sensors
    for gate_config in config[CONF_GATE_SENSORS]:
        gate = gate_config[CONF_GATE]
        
        if CONF_STILL_SENSOR in gate_config:
            still_sensor = await cg.get_variable(gate_config[CONF_STILL_SENSOR])
            cg.add(var.set_gate_still_sensor(gate, still_sensor))
        
        if CONF_MOVE_SENSOR in gate_config:
            move_sensor = await cg.get_variable(gate_config[CONF_MOVE_SENSOR])
            cg.add(var.set_gate_move_sensor(gate, move_sensor))
        
        if CONF_STILL_THRESHOLD in gate_config:
            still_threshold = await cg.get_variable(gate_config[CONF_STILL_THRESHOLD])
            cg.add(var.set_gate_still_threshold_number(gate, still_threshold))
        
        if CONF_MOVE_THRESHOLD in gate_config:
            move_threshold = await cg.get_variable(gate_config[CONF_MOVE_THRESHOLD])
            cg.add(var.set_gate_move_threshold_number(gate, move_threshold))
    
    # Configure max distance gate numbers
    if CONF_MAX_MOVE_DISTANCE_GATE in config:
        max_move_distance_gate = await cg.get_variable(config[CONF_MAX_MOVE_DISTANCE_GATE])
        cg.add(var.set_max_move_distance_gate_number(max_move_distance_gate))
    
    if CONF_MAX_STILL_DISTANCE_GATE in config:
        max_still_distance_gate = await cg.get_variable(config[CONF_MAX_STILL_DISTANCE_GATE])
        cg.add(var.set_max_still_distance_gate_number(max_still_distance_gate))