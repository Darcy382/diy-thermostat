import logging
import time
import requests
from user_settings import api_key, lat, lon, arduino_api_computer_address, city_id
from constants import *

NUM_TEMP_SENSORS = 2
ARDUINO_API_URL = "/thermostat/mode"
# TIMEOUT = (3, 8) # (Seconds to establish connections, Seconds to server response)
ADVANCED_WEATHER_PERIOD = 15
TIME_SYNC_PERIOD = 720

logging.basicConfig(filename='thermostat_data.csv', filemode='a', format='%(message)s')

def kelvinToF(kelvinTemp):
    if kelvinTemp is None:
        return None
    return ((kelvinTemp - 273.15) * 1.8) + 32.0

def toCsv(lst):
    output = []
    for item in lst:
        output.append(str(item))
    return ",".join(output)

def get_fields():
    fields = []
    fields.append(TIME)
    fields.append(ARDUINO_TIME)
    fields.append(THERMOSTAT_MODE)
    fields.append(FAN_SETTING)
    fields.append(TEMP_SET_POINT)
    fields.append(HEATER_RELAY)
    fields.append(AC_RELAY)
    fields.append(FAN_RELAY)
    for i in range(NUM_TEMP_SENSORS):
        fields.append(SENSOR_TEMP(i))
        fields.append(SENSOR_HUMIDITY(i))
        fields.append(SENSOR_HEAT_INDEX(i))
        fields.append(SENSOR_LAST_READ(i))
    fields.append(USE_REAL_FEEL)
    fields.append(TEMPERATURE_BOUND)
    fields.append(OUTDOOR_TEMP)
    fields.append(OUTDOOR_REAL_FEEL)
    fields.append(OUTDOOR_HUMIDITY)
    fields.append(OUTDOOR_UV_INDEX)
    fields.append(OUTDOOR_CLOUDS)
    fields.append(ERROR_MESSAGES)
    return fields

def log_fields(fields):
    logging.warning(toCsv(fields))

fields = get_fields()
log_fields(fields)

counter = 0
while True:
    log_entry = {ERROR_MESSAGES: []}
    time_sync = counter % TIME_SYNC_PERIOD == 0
    advanced_weather_call = counter % ADVANCED_WEATHER_PERIOD == 0
    try:
        if time_sync:
            arduino_request = requests.post(f"http://{arduino_api_computer_address}{ARDUINO_API_URL}", json={"time": True})
            counter = 0
        else:
            arduino_request = requests.get(f"http://{arduino_api_computer_address}{ARDUINO_API_URL}")
        if advanced_weather_call:
            weather_request = requests.get(f"https://api.openweathermap.org/data/2.5/onecall?lat={lat}&lon={lon}&exclude=minutely,hourly,daily,alerts&appid={api_key}")
        else:
            weather_request = requests.get(f"https://api.openweathermap.org/data/2.5/weather?id={city_id}&appid={api_key}")
        log_entry[TIME] = ((time.time()-18000) / 86400) + 25569
        if (arduino_request.status_code == 200):
            arduino_data = arduino_request.json()
            log_entry[THERMOSTAT_MODE] = (getThermostatMode[arduino_data.get("mode")])
            log_entry[FAN_SETTING] = (getFanSetting[arduino_data.get("fanSetting")])
            log_entry[HEATER_RELAY] = (getPowerState[arduino_data.get("heatRelay")])
            log_entry[AC_RELAY] = (getPowerState[arduino_data.get("acRelay")])
            log_entry[FAN_RELAY] = (getPowerState[arduino_data.get("fanRelay")])
            log_entry[TEMPERATURE_BOUND] = (getPowerState[arduino_data.get("tempBound")])
            log_entry[ARDUINO_TIME] = (getPowerState[arduino_data.get("time")])
            log_entry[USE_REAL_FEEL] = (getPowerState[arduino_data.get("useRealFeel")])
            log_entry[TEMP_SET_POINT] = (getPowerState[arduino_data.get("tempSetPoint")])
            sensor_data = arduino_data.get("sensors")
            i = 0
            for sensor_obj in sensor_data:
                log_entry[SENSOR_TEMP(i)] = sensor_obj.get("temperature")
                log_entry[SENSOR_HUMIDITY(i)] = sensor_obj.get("humidity")
                log_entry[SENSOR_HEAT_INDEX(i)] = sensor_obj.get("heat_idx")
                log_entry[SENSOR_LAST_READ(i)] = sensor_obj.get("last_read_time")
                i += 1
        else:
            error_message = f"Error with arduino api request, status code {arduino_request.status_code}"
            log_entry[ERROR_MESSAGES].append(error_message)
            print(error_message)

        if (weather_request.status_code == 200):
            if advanced_weather_call:
                weather_data = weather_request.json().get("current", {})
            else:
                weather_data = weather_request.json().get("main", {})
            log_entry[OUTDOOR_TEMP] = ("{:.1f}".format(kelvinToF(weather_data.get("temp"))))
            log_entry[OUTDOOR_REAL_FEEL] = ("{:.1f}".format(kelvinToF(weather_data.get("feels_like"))))
            log_entry[OUTDOOR_HUMIDITY] = weather_data.get("humidity")
            log_entry[OUTDOOR_UV_INDEX] = weather_data.get("uvi")
            log_entry[OUTDOOR_CLOUDS] = weather_data.get("clouds")
        else:
            error_message = f"Error with open weather api request, status code {arduino_request.status_code}"
            log_entry[ERROR_MESSAGES].append(error_message)
            print(error_message)
    except Exception as e:
        error_message = "The following error has occurred: {e}"
        log_entry[ERROR_MESSAGES].append(error_message)
        print(error_message)
        print(e)
    log_output_list = []
    for field in fields:
        log_output_list.append(log_entry.get(field))
    logging.warning(toCsv(log_output_list))
    time.sleep(15)
    counter += 1
