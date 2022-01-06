import logging
import time
import requests
from user_settings import api_key, lat, lon

NUM_TEMP_SENSORS = 2

filename = "university_records.csv"
getThermostatMode = {
    0: "OFF",
    1: "HEAT",
    2: "COOL"
}
getFanSetting = {
    0: "AUTO",
    1: "ALWAYS_ON"
}
POWER_STATE = {
    0: "OFF",
    1: "ON"
}
logging.basicConfig(filename='thermostat_data.csv', filemode='a', format='%(message)s')

def kelvinToF(kelvinTemp):
    return ((kelvinTemp - 273.15) * 1.8) + 32.0

def toCsv(lst):
    output = []
    for item in lst:
        output.append(str(item))
    return ",".join(output)

def log_fields():
    fields = []
    fields.append("Time")
    fields.append("Thermostat Mode")
    fields.append("Fan Setting")
    fields.append("Heater Relay")
    fields.append("AC Relay")
    fields.append("Fan Relay")
    for i in range(NUM_TEMP_SENSORS):
        fields.append(f"Sensor {i} Temp")
        fields.append(f"Sensor {i} Humidity")
        fields.append(f"Sensor {i} Heat Index")
    fields.append("Outdoor Temp")
    fields.append("Outdoor Real feel")
    fields.append("Outdoor Humidity")
    fields.append("Outdoor UV Index")
    fields.append("Outdoor Clouds")
    logging.warning(toCsv(fields))

# log_fields()
while True:
    arduino_data = requests.get("http://192.168.1.32:5000/thermostat/mode").json()
    weather_data = requests.get(f"https://api.openweathermap.org/data/2.5/onecall?lat={lat}&lon={lon}&exclude=minutely,hourly,daily,alerts&appid={api_key}").json()["current"]
    log = []
    log.append(((time.time()-18000) / 86400) + 25569)
    log.append(getThermostatMode[arduino_data.get("mode")])
    log.append(getFanSetting[arduino_data.get("fanSetting")])
    log.append(POWER_STATE[arduino_data["heatRelay"]])
    log.append(POWER_STATE[arduino_data["acRelay"]])
    log.append(POWER_STATE[arduino_data["fanRelay"]])
    sensor_data = arduino_data.get("sensors")
    for sensor_obj in sensor_data:
        log.append(sensor_obj.get("temperature"))
        log.append(sensor_obj.get("humidity"))
        log.append(sensor_obj.get("heat_idx"))
    log.append("{:.1f}".format(kelvinToF(weather_data.get("temp"))))
    log.append("{:.1f}".format(kelvinToF(weather_data.get("feels_like"))))
    log.append(weather_data.get("humidity"))
    log.append(weather_data.get("uvi"))
    log.append(weather_data.get("clouds"))
    logging.warning(toCsv(log))
    time.sleep(15)
