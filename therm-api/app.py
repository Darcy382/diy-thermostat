from flask import Flask, request
from flask_cors import CORS
import serial
import time
import json

app = Flask(__name__)
CORS(app)

app.config['CORS_HEADERS'] = 'Content-Type'

arduino = serial.Serial(port='/dev/cu.usbmodem1101', baudrate=9600, timeout=.1)


def ping_arduino(send_data):
    arduino.flushInput()
    arduino.flushOutput()
    arduino.write(json.dumps(send_data).encode('utf-8'))
    while arduino.in_waiting == 0:
        pass
    received_data = arduino.readline().decode('utf-8')
    return app.response_class(
        received_data,
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