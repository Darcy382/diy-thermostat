from flask import Flask, request
from flask_cors import CORS
import serial
import time
import json

app = Flask(__name__)
CORS(app)

app.config['CORS_HEADERS'] = 'Content-Type'

arduino = serial.Serial(port='/dev/cu.usbmodem2101', baudrate=9600, timeout=.1)

# def read_current_temp():
#     arduino.flushInput()
#     value = b''
#     while value == b'':
#         time.sleep(0.05)
#         value = arduino.readline()
#     return float(value)


# @app.route('/temperature')
# def get_current_temp():
#     data = json.dumps({'temp': str(read_current_temp())})
#     # data = json.dumps({'temp': 20})
#     return app.response_class(
#         data,
#         mimetype='applications/json'
#     )

@app.route('/thermostat/mode', methods = ['GET'])
def set_therm_mode():
    arduino.flushInput()
    arduino.flushOutput()
    arduino.write('5'.encode('utf-8'))
    while arduino.in_waiting == 0:
        pass
    mode = int(arduino.readline())
    return app.response_class(
        json.dumps({'mode': mode}),
        mimetype='applications/json'
    )
    
@app.route('/thermostat/mode', methods = ['POST'])
def get_therm_mode():
    new_mode = request.json['mode']
    arduino.flushInput()
    arduino.flushOutput()
    arduino.write((str(new_mode)).encode('utf-8'))
    while arduino.in_waiting == 0:
        pass
    mode = int(arduino.readline())
    return app.response_class(
        json.dumps({'mode': mode}),
        mimetype='applications/json'
    )

if __name__ == "__main__":
    app.run(host='192.168.1.32')