from flask import Flask, request, send_from_directory
from flask_cors import CORS
import serial
import time
import json
from constants import *
from user_settings import *

NUM_TEMP_SENSORS = 2
MAX_SCHEDULED_EVENTS = 4

app = Flask(__name__)
CORS(app)

app.config['CORS_HEADERS'] = 'Content-Type'
app.config["CSV_SERVICE"] = path

def floatToString(num):
    return "{:.2f}".format(num)

if not test_mode: 
    arduino = serial.Serial(port='/dev/cu.usbmodem14201', baudrate=9600, timeout=.1)

    def serial_print(string):
        encoded = str(string).encode('utf-8')
        print(encoded, end="")
        arduino.write(encoded)

    def serial_read():
        return arduino.read().decode('utf-8')

    def parseInt(lst):
        N = len(lst)
        i = 0
        while i < N and (not lst[i].isdigit()):
            i += 1
        start_idx = i
        while i < N and (lst[i].isdigit()):
            i += 1
        return int("".join(lst[start_idx:i])), lst[i:]

    def parseFloat(lst):
        N = len(lst)
        i = 0
        while i < N and (not (lst[i].isdigit() or lst[i] == ".")):
            i += 1
        start_idx = i
        while i < N and (lst[i].isdigit() or lst[i] == "."):
            i += 1
        return float("".join(lst[start_idx:i])), lst[i:]

    def ping_arduino(send_data):
        signal_chars = {
            MODE: 'M',
            FAN_SETTING: 'F',
            TEMP_BOUND: "B",
            TIME: 'T',
            USE_REAL_FEEL: "I",
            WEEKDAY_SCHEDULE_COOL: 'SCD',
            WEEKEND_SCHEDULE_COOL: 'SCE',
            WEEKDAY_SCHEDULE_HEAT: 'SHD',
            WEEKEND_SCHEDULE_HEAT: 'SHE',
        }

        if len(send_data) == 0:
            serial_print('P')
            
        for key, value in send_data.items():
            if key in signal_chars:
                if key == MODE:
                    serial_print(signal_chars[key])
                    serial_print(value)
                elif key == FAN_SETTING:
                    serial_print(signal_chars[key])
                    serial_print(value)
                elif key == TIME and value == True:
                    serial_print(signal_chars[key])
                    serial_print(int(time.time()) - 18000)
                elif key == TEMP_BOUND:
                    serial_print(signal_chars[key])
                    serial_print(floatToString(value))
                elif key == USE_REAL_FEEL:
                    serial_print(signal_chars[key])
                    serial_print(value)
                else:
                    N = len(value)
                    serial_print(signal_chars[key])
                    for i in range(N):
                        serial_print(' ')
                        serial_print(floatToString(value[i][START]))
                        serial_print(' ')
                        serial_print(floatToString(value[i][TEMP]))
                    # TODO: write null for the remaining 4 temperatures if they don't exist
        serial_print("*") # Marks the end of transmission to arduino

        print() # Prints a newline after print what was sent to the arduino
        while arduino.in_waiting == 0: 
            pass

        response = {}
        arduino_response = arduino.readall()
        print(arduino_response)
        result = list(arduino_response.decode('utf-8'))
        i = 0
        while len(result) > 0:
            char = result[0]
            if char == "C":
                response["sensors"] = []
                for i in range(NUM_TEMP_SENSORS):
                    sensorObj = {}
                    temp, result = parseFloat(result)
                    sensorObj["temperature"] = (temp)
                    humidity, result = parseFloat(result)
                    sensorObj["humidity"] = (humidity)
                    heat_idx, result = parseFloat(result)
                    sensorObj["heat_idx"] = (heat_idx)
                    last_read_time, result = parseInt(result)
                    sensorObj["last_read_time"] = last_read_time
                    response["sensors"].append(sensorObj)
            elif char == "I":
                use_real_feel, result = parseInt(result)
                response[USE_REAL_FEEL] = use_real_feel
            elif char == "B":
                temp_bound, result = parseFloat(result)
                response[TEMP_BOUND] = temp_bound
            elif char == "R":
                heatRelay, result = parseInt(result)
                response["heatRelay"] = heatRelay
                acRelay, result = parseInt(result)
                response["acRelay"] = acRelay
                fanRelay, result = parseInt(result)
                response["fanRelay"] = fanRelay
            elif char == "M":
                mode, result = parseInt(result)
                response[MODE] = mode
            elif char == "A":
                temp_set_point, result = parseFloat(result)
                response[TEMP_SET_POINT] = temp_set_point
            elif char == "F":
                fanSetting, result = parseInt(result)
                response["fanSetting"] = fanSetting
            elif char == "T":
                arduino_time, result = parseInt(result)
                response[TIME] = arduino_time
            elif char == "S":
                char2 = result[1]
                char3 = result[2]
                if char2 == "C":
                    if char3 == "D":
                        response[WEEKDAY_SCHEDULE_COOL] = []
                        for i in range(MAX_SCHEDULED_EVENTS):
                            start, result = parseFloat(result)
                            temp, result = parseFloat(result)
                            response[WEEKDAY_SCHEDULE_COOL].append({
                                START: start,
                                TEMP: temp
                            })
                    elif char3 == "E":
                        response[WEEKEND_SCHEDULE_COOL] = []
                        for i in range(MAX_SCHEDULED_EVENTS):
                            start, result = parseFloat(result)
                            temp, result = parseFloat(result)
                            response[WEEKEND_SCHEDULE_COOL].append({
                                START: start,
                                TEMP: temp
                            })
                    else:
                        raise Exception
                elif char2 == "H":
                    if char3 == "D":
                        response[WEEKDAY_SCHEDULE_HEAT] = []
                        for i in range(MAX_SCHEDULED_EVENTS):
                            start, result = parseFloat(result)
                            temp, result = parseFloat(result)
                            response[WEEKDAY_SCHEDULE_HEAT].append({
                                START: start,
                                TEMP: temp
                            })
                    elif char3 == "E":
                        response[WEEKEND_SCHEDULE_HEAT] = []
                        for i in range(MAX_SCHEDULED_EVENTS):
                            start, result = parseFloat(result)
                            temp, result = parseFloat(result)
                            response[WEEKEND_SCHEDULE_HEAT].append({
                                START: start,
                                TEMP: temp
                            })
                    else:
                        raise Exception
                else:
                    raise Exception
            elif char=="e":
                errorCode, result = parseInt(result)
                print(f"Arduino Error code # {errorCode} has occurred")
            else:
                # TODO: Add better checking for unknown letters here
                result = result[1:]
            
        return app.response_class(
            json.dumps(response),
            mimetype='applications/json'
        )


@app.route('/thermostat/mode', methods = ['GET'])
def set_therm_mode():
    if not test_mode:
        return ping_arduino({})
    else:
        time.sleep(0.5)
        return app.response_class(
            json.dumps(test_data),
            mimetype='applications/json'
        )


@app.route('/thermostat/mode', methods = ['POST'])
def get_therm_mode():
    # TODO: Check if the input is valid
    if not test_mode:
        return ping_arduino(request.json)
    else:
        time.sleep(0.5)
        print(request.json)
        for key, value in request.json.items():
            test_data[key] = value
        return app.response_class(
            json.dumps(test_data),
            mimetype='applications/json'
        )
    
@app.route("/thermostat/logging")
def getSensorLogging():
    return send_from_directory(app.config["CSV_SERVICE"], path="thermostat_data.csv", as_attachment=True)


if __name__ == "__main__":
    if host_ip is None:
        app.run(threaded=False)
    else: 
        app.run(host=host_ip, threaded=False)
