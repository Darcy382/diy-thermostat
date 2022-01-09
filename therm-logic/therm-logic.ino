#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TimeLib.h>
#include <TM1637Display.h>
#include <EEPROM.h>
#include <set.h>

#define FAN_PIN 4
#define AC_PIN 3
#define HEATER_PIN 2
#define COMMUNICATION_LIGHT 5
#define RADIO_LIGHT 14
#define HEAT_MODE_LIGHT 6
#define COOL_MODE_LIGHT 9
#define FAN_MODE_LIGHT 10
#define NUM_TEMP_SENSORS 2
#define CLK A1
#define DIO A2
#define MAX_SCHEDULED_EVENTS 4
#define ERROR_LIGHT A3

enum Thermostat_state_name { THERM_OFF=0, HEAT=1, COOL=2 };
enum Fan_setting_name { AUTO=0, ALWAYS_ON=1};
enum Power_state { OFF, ON };
float bound;
const unsigned long DEFAULT_TIME = 1640897447;
bool use_real_feel;

Set sensors_read;

RF24 radio(7, 8); // CE, CSN
TM1637Display display = TM1637Display(CLK, DIO);

uint64_t address[6] = {
  0x7878787878LL,
  0xB3B4B5B6F1LL,
  0xB3B4B5B6CDLL,
  0xB3B4B5B6A3LL,
  0xB3B4B5B60FLL,
  0xB3B4B5B605LL
};

struct PayloadStruct {
  char nodeID;
  float temperature;
  float humidity;
  float heat_idx;
};
PayloadStruct payload;

struct SensorReading {
  long last_read_time;
  float temperature;
  float humidity;
  float heat_idx;
};

float schedule_wend_cool_start[MAX_SCHEDULED_EVENTS];
float schedule_wend_cool_temp[MAX_SCHEDULED_EVENTS];

float schedule_wday_cool_start[MAX_SCHEDULED_EVENTS];
float schedule_wday_cool_temp[MAX_SCHEDULED_EVENTS];

float schedule_wend_heat_start[MAX_SCHEDULED_EVENTS];
float schedule_wend_heat_temp[MAX_SCHEDULED_EVENTS];

float schedule_wday_heat_start[MAX_SCHEDULED_EVENTS];
float schedule_wday_heat_temp[MAX_SCHEDULED_EVENTS];

int thermostat_state;
int fan_setting;
float target;
float avg_temp;
int eeAddress;

SensorReading sensors[NUM_TEMP_SENSORS];
bool received_all_temps;  

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100); // Increase this if having issues 
  
  thermostat_state = THERM_OFF;
  fan_setting = AUTO;
  target = 70;
  bound = 1;
  use_real_feel = false;

  pinMode(FAN_PIN, OUTPUT);
  pinMode(AC_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(COMMUNICATION_LIGHT, OUTPUT);
  pinMode(RADIO_LIGHT, OUTPUT);
  pinMode(HEAT_MODE_LIGHT, OUTPUT);
  pinMode(COOL_MODE_LIGHT, OUTPUT);
  pinMode(FAN_MODE_LIGHT, OUTPUT);
  pinMode(ERROR_LIGHT, OUTPUT);
  
  radio.begin();
  for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
    radio.openReadingPipe(i, address[i]);
  }
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  // Implement an automatic time sync?
  // setSyncProvider( requestSync);

  setTime(DEFAULT_TIME);

  display.clear();
  display.setBrightness(4);

  eeAddress = 0;
  eeAddress = readList(eeAddress, schedule_wend_cool_start);
  eeAddress = readList(eeAddress, schedule_wend_cool_temp);

  eeAddress = readList(eeAddress, schedule_wday_cool_start);
  eeAddress = readList(eeAddress, schedule_wday_cool_temp);

  eeAddress = readList(eeAddress, schedule_wend_heat_start);
  eeAddress = readList(eeAddress, schedule_wend_heat_temp);

  eeAddress = readList(eeAddress, schedule_wday_heat_start);
  eeAddress = readList(eeAddress, schedule_wday_heat_temp);
}

void loop() {
  // Receiving data from all sensors
  digitalWrite(RADIO_LIGHT, HIGH);
  sensors_read.clear();
  while ((sensors_read.count() < NUM_TEMP_SENSORS) && Serial.available() == 0) {
    if (radio.available()) {
      radio.read(&payload, sizeof(payload));
      if (0 <= payload.nodeID <= NUM_TEMP_SENSORS - 1) {
        sensors[payload.nodeID].temperature = payload.temperature;
        sensors[payload.nodeID].humidity = payload.humidity;
        sensors[payload.nodeID].heat_idx = payload.heat_idx;
        sensors[payload.nodeID].last_read_time = now();
        sensors_read.add(payload.nodeID);
      } else {
          digitalWrite(ERROR_LIGHT, HIGH);
          // An invalid nodeID has been received
      }
    }
  }
  digitalWrite(RADIO_LIGHT, LOW);
  
  // Checking for computer input
  bool computer_input = false;
  bool new_schedule = false;
  while (Serial.available() != 0) {
    digitalWrite(COMMUNICATION_LIGHT, HIGH);
    computer_input = true;
    char header = Serial.read();
    switch (header)
    {
      case 'P':
        break;
      case '*':
        break;
      case 'M':
        thermostat_state = Serial.parseInt();
        break;
      case 'F':
        fan_setting = Serial.parseInt();
        break;
      case 'T':
        setTime(Serial.parseInt());
        break;
      case 'I':
        use_real_feel = Serial.parseInt();
        break;
      case 'S':
        new_schedule = true;
        char header2;
        char header3;
        while (Serial.available() < 2) {}
        header2 = Serial.read();
        header3 = Serial.read();
        float *setting_schedule_start;
        float *setting_schedule_temp;
        switch (header2)
        {
          case 'H':
            switch (header3)
            {
              case 'D':
                setting_schedule_start = schedule_wday_heat_start;
                setting_schedule_temp = schedule_wday_heat_temp;
                break;
              case 'E':
                setting_schedule_start = schedule_wend_heat_start;
                setting_schedule_temp = schedule_wend_heat_temp;
                break;
              default:
                Serial.write("e");
                Serial.print(1);
                digitalWrite(ERROR_LIGHT, HIGH);
                break;
            }
            break;
          case 'C':
            switch (header3)
            {
              case 'D':
                setting_schedule_start = schedule_wday_cool_start;
                setting_schedule_temp = schedule_wday_cool_temp;
                break;
              case 'E':
                setting_schedule_start = schedule_wend_cool_start;
                setting_schedule_temp = schedule_wend_cool_temp;
                break;
              default:
                Serial.print("e");
                Serial.print(2);
                digitalWrite(ERROR_LIGHT, HIGH);
                break;
            }
            break;
          default:
            Serial.print("e");
            Serial.print(3);
            digitalWrite(ERROR_LIGHT, HIGH);
            break;
        }
        for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
          setting_schedule_start[i] = Serial.parseFloat();
          setting_schedule_temp[i] = Serial.parseFloat();
        }
        break;
      default:
        Serial.print("e");
        Serial.print(4);
        digitalWrite(ERROR_LIGHT, HIGH);
        break;
    }
  }
  if (computer_input) {
    // Print out the current temp sensor readings T 12 12 12 12
    Serial.write("C");
    for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
      Serial.write(" ");
      Serial.print(sensors[i].temperature, 2);
      Serial.write(" ");
      Serial.print(sensors[i].humidity, 2);
      Serial.write(" ");
      Serial.print(sensors[i].heat_idx, 2);
      Serial.write(" ");
      Serial.print(sensors[i].last_read_time);
    }

    // Print out the relay states
    Serial.write("R");
    Serial.print(heater_on());
    Serial.write(" ");
    Serial.print(ac_on());
    Serial.write(" ");
    Serial.print(fan_on());
    // Print out the thermostat mode 
    Serial.write("M");
    Serial.print(thermostat_state);

    // Print out the fan setting mode 
    Serial.write("F");
    Serial.print(fan_setting);

    Serial.write("I");
    Serial.print(use_real_feel);
    // Print out the current time
    Serial.write("T");
    Serial.print(now());

    // Print out all of the schedules
    Serial.write("SCD");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      Serial.write(" ");
      Serial.print(schedule_wday_cool_start[i]);
      Serial.write(" ");
      Serial.print(schedule_wday_cool_temp[i]);
    }
    Serial.write("SCE");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      Serial.write(" ");
      Serial.print(schedule_wend_cool_start[i]);
      Serial.write(" ");
      Serial.print(schedule_wend_cool_temp[i]);
    }
    Serial.write("SHD");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      Serial.write(" ");
      Serial.print(schedule_wday_heat_start[i]);
      Serial.write(" ");
      Serial.print(schedule_wday_heat_temp[i]);
    }
    Serial.write("SHE");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      Serial.write(" ");
      Serial.print(schedule_wend_heat_start[i]);
      Serial.write(" ");
      Serial.print(schedule_wend_heat_temp[i]);
    }
    digitalWrite(COMMUNICATION_LIGHT, LOW);
    Serial.println();
  }
  
  if (new_schedule) {
    eeAddress = 0;
    eeAddress = writeList(eeAddress, schedule_wend_cool_start);
    eeAddress = writeList(eeAddress, schedule_wend_cool_temp);

    eeAddress = writeList(eeAddress, schedule_wday_cool_start);
    eeAddress = writeList(eeAddress, schedule_wday_cool_temp);

    eeAddress = writeList(eeAddress, schedule_wend_heat_start);
    eeAddress = writeList(eeAddress, schedule_wend_heat_temp);

    eeAddress = writeList(eeAddress, schedule_wday_heat_start);
    eeAddress = writeList(eeAddress, schedule_wday_heat_temp);
  }
  
  // Checking schedule for target temperature
  if (timeStatus() == timeNotSet) {
      digitalWrite(ERROR_LIGHT, HIGH);
  }
  float *current_schedule_start;
  float *current_schedule_temp;
  switch (thermostat_state)
  {
    case HEAT:
      if ((2 <= weekday()) && (weekday() <= 6)) { // Weekday
        current_schedule_start = schedule_wday_heat_start;
        current_schedule_temp = schedule_wday_heat_temp;
      } else {
        current_schedule_start = schedule_wend_heat_start;
        current_schedule_temp = schedule_wend_heat_temp;
      }
      break;
    case COOL: 
      if ((2 <= weekday()) && (weekday() <= 6)) { // Weekday
        current_schedule_start = schedule_wday_cool_start;
        current_schedule_temp = schedule_wday_cool_temp;
      } else {
        current_schedule_start = schedule_wend_cool_start;
        current_schedule_temp = schedule_wend_cool_temp;
      }
      break;
    default:
      current_schedule_start = NULL;
      current_schedule_temp = NULL;
      break;
  }
  
  float current_hours = hour() + ((minute() + (second() / 60.)) / 60.);
  int i = 0;
  while (
    current_schedule_start != NULL &&
    i < MAX_SCHEDULED_EVENTS &&
    current_hours >= current_schedule_start[i]
  ) {
    target = current_schedule_temp[i]; 
    i++;
  }
  display.showNumberDec(target, false);

  // Calculating average temperature
  float sum = 0;
  int sensors_used = 0;
  for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
    if ((now() - sensors[i].last_read_time) < 1200) {
      if (use_real_feel) {
        sum += sensors[i].heat_idx;  
      } else {
        sum += sensors[i].temperature;
      }
      sensors_used++;
    }
  }
  if (sensors_used == 0) {
    // If no temperature sensors are responding, turn everything off
    if (ac_on()) {
      turn_ac(OFF);
    }
    if (heater_on()) {
      turn_heater(OFF);
    }
    if (fan_on()) {
      turn_fan(OFF);
    }
    return;
  }

  avg_temp = sum / sensors_used;

  // Starting classical thermostat logic
  switch(thermostat_state) {
    case HEAT :
      digitalWrite(HEAT_MODE_LIGHT, HIGH);
      digitalWrite(COOL_MODE_LIGHT, LOW);
      
      if (ac_on()) {
        turn_ac(OFF);
      }

      if (!heater_on() && avg_temp < target - bound) {
        turn_heater(ON);
      }
      else if (heater_on() && avg_temp > target + bound) {
        turn_heater(OFF);
      }
      break;
    case COOL:
      digitalWrite(HEAT_MODE_LIGHT, LOW);
      digitalWrite(COOL_MODE_LIGHT, HIGH);

      if (heater_on()) {
        turn_heater(OFF);
      }

      if (!ac_on() && avg_temp > target + bound) {
        turn_ac(ON);
      }
      else if (ac_on() && avg_temp < target - bound) {
        turn_ac(OFF);
      }
      break;
    case THERM_OFF :
      digitalWrite(HEAT_MODE_LIGHT, LOW);
      digitalWrite(COOL_MODE_LIGHT, LOW);
      
      if (heater_on()) {
        turn_heater(OFF);
      }
      if (ac_on()) {
        turn_ac(OFF);
      }
      break;
    default :
      digitalWrite(ERROR_LIGHT, HIGH);
      // The Thermostat is in an invalid state
      break;
  }
  switch (fan_setting)
  {
  case ALWAYS_ON:
    digitalWrite(FAN_MODE_LIGHT, HIGH);
    if (!fan_on()) {
      turn_fan(ON);
    }
    break;
  case AUTO:
    digitalWrite(FAN_MODE_LIGHT, LOW);
    if (ac_on()) {
      turn_fan(ON);
    } else {
      turn_fan(OFF);
    }
    break;
  default:
    digitalWrite(ERROR_LIGHT, HIGH);
    // The fan setting is invalid
    break;
  }

  int k = 0;
  while (Serial.available() == 0 and k < 20) {
    delay(50);
    k++;
  }
}

void turn_fan(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(FAN_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(FAN_PIN, LOW);
  } else {
    // Invalid power state
    digitalWrite(ERROR_LIGHT, HIGH);
  }
}

void turn_heater(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(HEATER_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(HEATER_PIN, LOW);
  } else {
    // Invalid power state
    digitalWrite(ERROR_LIGHT, HIGH);
  }
}

void turn_ac(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(AC_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(AC_PIN, LOW);
  } else {
    // Invalid power state
    digitalWrite(ERROR_LIGHT, HIGH);
  }
}

bool fan_on() {
  if (digitalRead(FAN_PIN) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool heater_on() {
  if (digitalRead(HEATER_PIN) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool ac_on() {
  if (digitalRead(AC_PIN) == HIGH) {
    return true;
  } else {
    return false;
  }
}

int writeList(int eeAddress, float ary[MAX_SCHEDULED_EVENTS]) {
  int incr = sizeof(float);
  for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
    EEPROM.put(eeAddress, ary[i]);
    eeAddress+=incr;
  }
  return eeAddress;
}

int readList(int eeAddress, float ary[MAX_SCHEDULED_EVENTS]) {
  int incr = sizeof(float);
  for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
    EEPROM.get(eeAddress, ary[i]);
    eeAddress+=incr;
  }
  return eeAddress;
}
