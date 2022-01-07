TIME = "time"
THERMOSTAT_MODE = "thermostat_mode"
FAN_SETTING = "fan_setting"
HEATER_RELAY = "heater_relay"
AC_RELAY = "ac_relay"
FAN_RELAY = "fan_relay"
OUTDOOR_TEMP = "outdoor_temp"
OUTDOOR_REAL_FEEL = "outdoor_real_feel"
OUTDOOR_HUMIDITY = "outdoor_humidity"
OUTDOOR_UV_INDEX = "outdoor_uv_index"
OUTDOOR_CLOUDS = "outdoor_clouds"
ERROR_MESSAGES = "error_messages"

def SENSOR_TEMP(sensor_num):
    return f"sensor_{sensor_num}_temp"

def SENSOR_HUMIDITY(sensor_num):
    return f"sensor_{sensor_num}_humidity"

def SENSOR_HEAT_INDEX(sensor_num):
    return f"sensor_{sensor_num}_heat_index"

getThermostatMode = {
    0: "OFF",
    1: "HEAT",
    2: "COOL",
    None: None
}
getFanSetting = {
    0: "AUTO",
    1: "ALWAYS_ON",
    None: None
}
getPowerState = {
    0: "OFF",
    1: "ON",
    None: None
}