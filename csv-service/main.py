import logging
import time
import json 
import requests
fields = ['Time','Sensor0Temp', 'Sensor1Temp', 'ThermostatMode']

filename = "university_records.csv"

logging.basicConfig(filename='thermostat_data.csv', filemode='a', format='%(message)s')

def toCsv(lst):
    output = []
    for item in lst:
        output.append(str(item))
    return ",".join(output)

def log_fields():
    logging.warning(toCsv(fields))


# log_fields()
while True:
    time.sleep(15)
    data = requests.get("http://192.168.1.32:5000/thermostat/mode").json()
    log = []
    log.append(time.time())
    temps = data.get("temperatures")
    if len(temps) < 2: 
        temps = [None, None]
    log.append(temps[0])
    log.append(temps[1])
    log.append(data.get("mode"))
    logging.warning(toCsv(log))
