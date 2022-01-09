WEEKDAY_SCHEDULE_COOL = "weekdayScheduleCool"
WEEKEND_SCHEDULE_COOL = "weekendScheduleCool"
WEEKDAY_SCHEDULE_HEAT = "weekdayScheduleHeat"
WEEKEND_SCHEDULE_HEAT = "weekendScheduleHeat"


test_data = {
    "sensors": [
        {
            "temperature": 67.82,
            "humidity": 40.0,
            "heat_idx": 66.18
        },
        {
            "temperature": 69.26,
            "humidity": 41.0,
            "heat_idx": 67.81
        }
    ],
    "heatRelay": 0,
    "acRelay": 0,
    "fanRelay": 0,
    "mode": 1,
    "fanSetting": 0,
    "time": 1641581769,
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
            "temp": 68.0
        },
        {
            "start": 12.02,
            "temp": 68.0
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
            "temp": 68.0
        },
        {
            "start": 10.0,
            "temp": 68.0
        },
        {
            "start": 14.0,
            "temp": 68.0
        },
        {
            "start": 22.0,
            "temp": 62.0
        }
    ]
}