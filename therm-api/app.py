from flask import Flask
from flask_cors import CORS
import serial
import time
import json

app = Flask(__name__)
CORS(app)

arduino = serial.Serial(port='/dev/cu.usbmodem11101', baudrate=9600, timeout=.1)

app.config['CORS_HEADERS'] = 'Content-Type'

def read_current_temp():
    arduino.flushInput()
    value = b''
    while value == b'':
        time.sleep(0.05)
        value = arduino.readline()
    return float(value)


@app.route('/temp')
def get_current_temp():
    data = json.dumps({'temp': str(read_current_temp())})
    return app.response_class(
        data,
        mimetype='applications/json'
    )

if __name__ == "__main__":
    app.run(host='192.168.1.32')