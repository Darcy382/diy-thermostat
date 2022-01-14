WEEKDAY_SCHEDULE_COOL = "weekdayScheduleCool"
WEEKEND_SCHEDULE_COOL = "weekendScheduleCool"
WEEKDAY_SCHEDULE_HEAT = "weekdayScheduleHeat"
WEEKEND_SCHEDULE_HEAT = "weekendScheduleHeat"
MODE = "mode"
FAN_SETTING = "fanSetting"
TIME = "time"
START = "start"
TEMP = "temp"
TEMP_BOUND = "tempBound"
USE_REAL_FEEL = "useRealFeel"
TEMP_SET_POINT = "tempSetPoint"

test_data = {
    "sensors": [
        {
            "temperature": 70.34,
            "humidity": 44.0,
            "heat_idx": 69.14,
            "last_read_time": 1642080326
        },
        {
            "temperature": 69.26,
            "humidity": 40.0,
            "heat_idx": 67.77,
            "last_read_time": 1642080326
        }
    ],
    "heatRelay": 0,
    "acRelay": 0,
    "fanRelay": 0,
    "mode": 1,
    "tempSetPoint": 70.0,
    "fanSetting": 0,
    "useRealFeel": 0,
    "tempBound": 0.5,
    "time": 1642080326,
    "weekdayScheduleCool": [
        {
            "start": 1.0,
            "temp": 71.0
        },
        {
            "start": 4.0,
            "temp": 73.0
        },
        {
            "start": 7.0,
            "temp": 74.0
        },
        {
            "start": 10.0,
            "temp": 73.0
        }
    ],
    "weekendScheduleCool": [
        {
            "start": 1.0,
            "temp": 62.0
        },
        {
            "start": 2.0,
            "temp": 63.0
        },
        {
            "start": 11.0,
            "temp": 64.0
        },
        {
            "start": 17.0,
            "temp": 65.0
        }
    ],
    "weekdayScheduleHeat": [
        {
            "start": 6.5,
            "temp": 70.0
        },
        {
            "start": 12.02,
            "temp": 70.0
        },
        {
            "start": 21.52,
            "temp": 62.0
        },
        {
            "start": 22.0,
            "temp": 62.0
        }
    ],
    "weekendScheduleHeat": [
        {
            "start": 7.0,
            "temp": 70.0
        },
        {
            "start": 10.0,
            "temp": 70.0
        },
        {
            "start": 14.0,
            "temp": 70.0
        },
        {
            "start": 22.0,
            "temp": 62.0
        }
    ]
}