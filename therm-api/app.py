import sys
from flask import Flask, request
from flask_cors import CORS
import serial
import time
import json

NUM_TEMP_SENSORS = 2
MAX_SCHEDULED_EVENTS = 4

app = Flask(__name__)
CORS(app)

app.config['CORS_HEADERS'] = 'Content-Type'

arduino = serial.Serial(port='/dev/cu.usbmodem101', baudrate=9600, timeout=.1)

def serial_print(string):
    print(arduino.out_waiting)
    arduino.write(str(string).encode('utf-8'))

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
    arduino.reset_input_buffer()
    arduino.reset_output_buffer()
    signal_chars = {
        "mode": 'M',
        "time": 'T',
        "weekdayScheduleCold": 'SCD',
        "weekendScheduleCold": 'SCE',
        "weekdayScheduleHeat": 'SHD',
        "weekendScheduleHeat": 'SHE',
    }

    if len(send_data) == 0:
        serial_print('R')
        
    for key, value in send_data.items():
        if key in signal_chars:
            if key == "mode":
                serial_print(signal_chars[key])
                serial_print(value)
            elif key == "time" and value == True:
                serial_print(signal_chars[key])
                serial_print(int(time.time()) - 18000)
            else:
                N = len(value)
                serial_print(signal_chars[key])
                for i in range(N):
                    serial_print(' ')
                    serial_print(value[i]["start"])
                    serial_print(' ')
                    serial_print(value[i]["temp"])
                # TODO: write null for the remaining 4 temperatures if they exist

    while arduino.in_waiting == 0:
        pass
    response = {}
    result = list(arduino.readall().decode('utf-8'))
    i = 0
    while len(result) > 0:
        char = result[0]
        if char == "C":
            response["temperatures"] = []
            for i in range(NUM_TEMP_SENSORS):
                temp, result = parseFloat(result)
                response["temperatures"].append(temp)
        elif char == "M":
            mode, result = parseInt(result)
            response["mode"] = mode
        elif char == "T":
            arduino_time, result = parseInt(result)
            response["time"] = arduino_time
        elif char == "S":
            char2 = result[1]
            char3 = result[2]
            if char2 == "C":
                if char3 == "D":
                    response["weekdayScheduleCold"] = []
                    for i in range(MAX_SCHEDULED_EVENTS):
                        start, result = parseFloat(result)
                        temp, result = parseFloat(result)
                        response["weekdayScheduleCold"].append({
                            "start": start,
                            "temp": temp
                        })
                elif char3 == "E":
                    response["weekendScheduleCold"] = []
                    for i in range(MAX_SCHEDULED_EVENTS):
                        start, result = parseFloat(result)
                        temp, result = parseFloat(result)
                        response["weekendScheduleCold"].append({
                            "start": start,
                            "temp": temp
                        })
                else:
                    raise Exception
            elif char2 == "H":
                if char3 == "D":
                    response["weekdayScheduleHeat"] = []
                    for i in range(MAX_SCHEDULED_EVENTS):
                        start, result = parseFloat(result)
                        temp, result = parseFloat(result)
                        response["weekdayScheduleHeat"].append({
                            "start": start,
                            "temp": temp
                        })
                elif char3 == "E":
                    response["weekendScheduleHeat"] = []
                    for i in range(MAX_SCHEDULED_EVENTS):
                        start, result = parseFloat(result)
                        temp, result = parseFloat(result)
                        response["weekendScheduleHeat"].append({
                            "start": start,
                            "temp": temp
                        })
                else:
                    raise Exception
            else:
                raise Exception
        else:
            result = result[1:]
        
    return app.response_class(
        json.dumps(response),
        mimetype='applications/json'
    )


@app.route('/thermostat/mode', methods = ['GET'])
def set_therm_mode():
    return ping_arduino({})
    

@app.route('/thermostat/mode', methods = ['POST'])
def get_therm_mode():
    # TODO: Check if the input is valid
    return ping_arduino(request.json)


if __name__ == "__main__":
    app.run(host='192.168.1.32')
