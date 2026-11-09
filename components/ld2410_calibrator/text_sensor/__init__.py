import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)

DEPENDENCIES = ["ld2410_calibrator"]

from .. import ld2410_calibrator_ns, LD2410CalibratorComponent

# Configuration constants
CONF_LD2410_CALIBRATOR_ID = "ld2410_calibrator_id"

# Configuration keys
CONF_CALIBRATION_STATUS = "calibration_status"

# Text Sensor classes
CalibrationStatusTextSensor = ld2410_calibrator_ns.class_("CalibrationStatusTextSensor", text_sensor.TextSensor,  cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_LD2410_CALIBRATOR_ID): cv.use_id(LD2410CalibratorComponent),
    
    # Calibration Status Text Sensor
    cv.Optional(CONF_CALIBRATION_STATUS): text_sensor.text_sensor_schema(
        CalibrationStatusTextSensor,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:information",
    ),
})

async def to_ld2410_calibrator_text_sensor_code(text_sensor_config, parent_var):
    text_sensor_var = await text_sensor.new_text_sensor(text_sensor_config)
    await cg.register_component(text_sensor_var, text_sensor_config)
    await cg.register_parented(text_sensor_var, parent_var)

async def to_code(config):
    parent_var = await cg.get_variable(config[CONF_LD2410_CALIBRATOR_ID])

    if text_sensor_config := config.get(CONF_CALIBRATION_STATUS):
        await to_ld2410_calibrator_text_sensor_code(text_sensor_config, parent_var)
