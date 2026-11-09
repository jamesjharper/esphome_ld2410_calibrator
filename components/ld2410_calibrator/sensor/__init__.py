import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
    UNIT_PERCENT,
    ICON_PERCENT,
)

DEPENDENCIES = ["ld2410_calibrator"]

from .. import ld2410_calibrator_ns, LD2410CalibratorComponent

# Configuration constants
CONF_LD2410_CALIBRATOR_ID = "ld2410_calibrator_id"

# Configuration keys
CONF_CALIBRATION_PROGRESS = "calibration_progress"

# Sensor classes
CalibrationProgressSensor = ld2410_calibrator_ns.class_("CalibrationProgressSensor", sensor.Sensor)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_LD2410_CALIBRATOR_ID): cv.use_id(LD2410CalibratorComponent),
    
    # Calibration Progress Sensor (percentage)
    cv.Optional(CONF_CALIBRATION_PROGRESS): sensor.sensor_schema(
        CalibrationProgressSensor,
        unit_of_measurement=UNIT_PERCENT,
        icon=ICON_PERCENT,
        accuracy_decimals=1,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    ),
})

async def to_ld2410_calibrator_sensor_code(sensor_config, parent_var):
    sensor_var = await sensor.new_sensor(sensor_config)
    await cg.register_parented(sensor_var, parent_var)

async def to_code(config):
    parent_var = await cg.get_variable(config[CONF_LD2410_CALIBRATOR_ID])

    if sensor_config := config.get(CONF_CALIBRATION_PROGRESS):
        await to_ld2410_calibrator_sensor_code(sensor_config, parent_var)
