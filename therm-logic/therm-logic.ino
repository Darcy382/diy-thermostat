#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TimeLib.h>
#include <TM1637Display.h>

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

enum Thermostat_state_name { THERM_OFF=0, HEAT=1, COOL=2, FAN=3 };
enum Power_state { OFF, ON };
float bound = 1;
const unsigned long DEFAULT_TIME = 1640897447 - 18000;

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

float schedule_wend_cool_start[MAX_SCHEDULED_EVENTS] = {7, 8, 16, 21}; 
float schedule_wend_cool_temp[MAX_SCHEDULED_EVENTS] = {70, 75, 72, 73};

float schedule_wday_cool_start[MAX_SCHEDULED_EVENTS] = {7, 8, 16, 21}; 
float schedule_wday_cool_temp[MAX_SCHEDULED_EVENTS] = {70, 75, 72, 73};

float schedule_wend_heat_start[MAX_SCHEDULED_EVENTS] = {7, 8, 16, 21}; 
float schedule_wend_heat_temp[MAX_SCHEDULED_EVENTS] = {72, 68, 70, 65};

float schedule_wday_heat_start[MAX_SCHEDULED_EVENTS] = {7, 8, 16, 21}; 
float schedule_wday_heat_temp[MAX_SCHEDULED_EVENTS] = {72, 68, 70, 65};

int thermostat_state;
float target;
float avg_temp;
float temperatures[NUM_TEMP_SENSORS];
bool received_all_temps;  

void setup() {
  Serial.begin(9600);
  
  thermostat_state = THERM_OFF;
  target = 72;

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

  // setSyncProvider( requestSync); Not sure what this does

  setTime(DEFAULT_TIME);

  display.clear();
  display.setBrightness(7);

  Serial.setTimeout(1000); // Set to 100 or 1000 if having issues
}

void loop() {
  // Receiving temperatures from all sensors
  for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
    temperatures[i] = NULL;
  }
  digitalWrite(RADIO_LIGHT, HIGH);
  received_all_temps = false;

  while (!received_all_temps) {
    if (radio.available()) {
      radio.read(&payload, sizeof(payload));
      if (0 <= payload.nodeID <= NUM_TEMP_SENSORS - 1) {
        temperatures[payload.nodeID] = payload.temperature;
      } else {
          digitalWrite(ERROR_LIGHT, HIGH);
          // An invalid nodeID has been received
      }
    }

    received_all_temps = true;
    for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
      if (temperatures[i] == NULL) {
        received_all_temps = false;
      }
    }
  }
  digitalWrite(RADIO_LIGHT, LOW);
  
  // Checking for computer input
  bool computer_input = false;
  while (Serial.available() != 0) {
    digitalWrite(COMMUNICATION_LIGHT, HIGH);
    computer_input = true;
    char header = Serial.read();
    switch (header)
    {
      case 'M':
        thermostat_state = Serial.parseInt();
        break;
      case 'T':
        setTime(Serial.parseInt());
      case 'S':
        char header2 = Serial.read();
        char header3 = Serial.read();
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
                digitalWrite(ERROR_LIGHT, HIGH);
                break;
            }
            break;
          default:
            digitalWrite(ERROR_LIGHT, HIGH);
            break;
        }
        for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
          setting_schedule_start[i] = Serial.parseFloat();
          setting_schedule_temp[i] = Serial.parseFloat();
        }
      default:
        digitalWrite(ERROR_LIGHT, HIGH);
        break;
    }
  }
  if (computer_input) {
    // TODO: Print out return information in the Serial
    digitalWrite(COMMUNICATION_LIGHT, LOW);
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
    current_schedule_start[i] != NULL &&
    current_hours >= current_schedule_start[i]
  ) {
    target = current_schedule_temp[i]; 
    i++;
  }
  display.showNumberDec(target, false);


  // Calculating average temperature
  float sum = 0;
  for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
    sum += temperatures[i];
  }
  avg_temp = sum / NUM_TEMP_SENSORS;

  // Starting classical thermostat logic
  switch(thermostat_state) {
    case HEAT :
      digitalWrite(HEAT_MODE_LIGHT, HIGH);
      digitalWrite(COOL_MODE_LIGHT, LOW);
      digitalWrite(FAN_MODE_LIGHT, LOW);
      
      if (ac_on()) {
        turn_ac(OFF);
      }
      if (fan_on()) {
        turn_fan(OFF);
      }

      if (!heater_on() && avg_temp < target - bound) {
        turn_heater(ON);
      }
      else if (heater_on() && avg_temp > target + bound) {
        turn_heater(OFF);
      }
      break;
    case COOL:
      // TODO: Turn the fan off when fan mode exited?
      digitalWrite(HEAT_MODE_LIGHT, LOW);
      digitalWrite(COOL_MODE_LIGHT, HIGH);
      digitalWrite(FAN_MODE_LIGHT, LOW);

      if (heater_on()) {
        turn_heater(OFF);
      }

      if (!ac_on() && avg_temp > target + bound) {
        turn_fan(ON);
        turn_ac(ON);
      }
      else if (ac_on() && avg_temp < target - bound) {
        turn_fan(OFF);
        turn_ac(OFF);
      }
      break;
    case FAN :
      digitalWrite(HEAT_MODE_LIGHT, LOW);
      digitalWrite(COOL_MODE_LIGHT, LOW);
      digitalWrite(FAN_MODE_LIGHT, HIGH);
      
      if (heater_on()) {
        turn_heater(OFF);
      }
      if (ac_on()) {
        turn_ac(OFF);
      }
      
      if(!fan_on()) {
        turn_fan(ON);
      }
      break;
    case THERM_OFF :
      digitalWrite(HEAT_MODE_LIGHT, LOW);
      digitalWrite(COOL_MODE_LIGHT, LOW);
      digitalWrite(FAN_MODE_LIGHT, LOW);

      if (fan_on()) {
        turn_fan(OFF);
      }
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
  delay(1000);
}

void turn_fan(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(FAN_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(FAN_PIN, LOW);
  } else {
    // Turn on error light
    // Invalid power state
  }
}

void turn_heater(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(HEATER_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(HEATER_PIN, LOW);
  } else {
    // Turn on error light
    // Invalid power state
  }
}

void turn_ac(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(AC_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(AC_PIN, LOW);
  } else {
    // Turn on error light
    // Invalid power state
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
