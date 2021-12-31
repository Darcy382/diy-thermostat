#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ArduinoJson.h>
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
StaticJsonDocument<768> doc;
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

struct EventStruct {
  bool name;
  float startHour;
  float temp;
};
EventStruct weekdayScheduleCool[MAX_SCHEDULED_EVENTS];
EventStruct weekendScheduleCool[MAX_SCHEDULED_EVENTS];
EventStruct weekdayScheduleHeat[MAX_SCHEDULED_EVENTS];
EventStruct weekendScheduleHeat[MAX_SCHEDULED_EVENTS];

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

  weekendScheduleCool[0].startHour = 7;
  weekendScheduleCool[0].temp = 73;
  weekendScheduleCool[1].startHour = 8;
  weekendScheduleCool[1].temp = 70;
  weekendScheduleCool[2].startHour = 9;
  weekendScheduleCool[2].temp = 81;
  weekendScheduleCool[3].startHour = NULL;
  weekendScheduleCool[3].temp = NULL;
  
  weekdayScheduleCool[0].startHour = 7;
  weekdayScheduleCool[0].temp = 73;
  weekdayScheduleCool[1].startHour = 8;
  weekdayScheduleCool[1].temp = 70;
  weekdayScheduleCool[2].startHour = 9;
  weekdayScheduleCool[2].temp = 80;
  weekdayScheduleCool[3].startHour = NULL;
  weekdayScheduleCool[3].temp = NULL;

  weekendScheduleHeat[0].startHour = 7;
  weekendScheduleHeat[0].temp = 70;
  weekendScheduleHeat[1].startHour = 8;
  weekendScheduleHeat[1].temp = 73;
  weekendScheduleHeat[2].startHour = 21;
  weekendScheduleHeat[2].temp = 62;
  weekendScheduleHeat[3].startHour = NULL;
  weekendScheduleHeat[3].temp = NULL;

  weekdayScheduleHeat[0].startHour = 7;
  weekdayScheduleHeat[0].temp = 70;
  weekdayScheduleHeat[1].startHour = 8;
  weekdayScheduleHeat[1].temp = 73;
  weekdayScheduleHeat[2].startHour = 21;
  weekdayScheduleHeat[2].temp = 62;
  weekdayScheduleHeat[3].startHour = NULL;
  weekdayScheduleHeat[3].temp = NULL;

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
  if (Serial.available() != 0) {
    digitalWrite(COMMUNICATION_LIGHT, HIGH);
    Serial.parseInt();
    Serial.write(1);
    while (!Serial.available()) {}
    // Deserializing computer json input
    DeserializationError error = deserializeJson(doc, Serial);
    if (error) {
      digitalWrite(ERROR_LIGHT, HIGH);
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    JsonVariant mode_input = doc["mode"];
    if (!mode_input.isNull()) {
      thermostat_state = doc["mode"];
    }
    JsonVariant time_input = doc["time"];
    if (!time_input.isNull()) {
      setTime(doc["time"]);
    }

     JsonVariant weekendScheduleCool_input = doc["weekendScheduleCool"];
     if (!weekendScheduleCool_input.isNull()) {
       for (int i = 0; i <= MAX_SCHEDULED_EVENTS; i++) {
         weekendScheduleCool[i].startHour = doc["weekendScheduleCool"][i]["start_hour"];
         weekendScheduleCool[i].temp = doc["weekendScheduleCool"][i]["temp"];
         i++;
       }
     }
    JsonVariant weekdayScheduleCool_input = doc["weekdayScheduleCool"];
    if (!weekdayScheduleCool_input.isNull()) {
      for (int i = 0; i <= MAX_SCHEDULED_EVENTS; i++) {
        weekdayScheduleCool[i].startHour = doc["weekdayScheduleCool"][i]["start_hour"];
        weekdayScheduleCool[i].temp = doc["weekdayScheduleCool"][i]["temp"];
        i++;
      }
    }
    JsonVariant weekendScheduleHeat_input = doc["weekendScheduleHeat"];
    if (!weekendScheduleHeat_input.isNull()) {
      for (int i = 0; i <= MAX_SCHEDULED_EVENTS; i++) {
        weekendScheduleHeat[i].startHour = doc["weekendScheduleHeat"][i]["start_hour"];
        weekendScheduleHeat[i].temp = doc["weekendScheduleHeat"][i]["temp"];
        i++;
      }
    }
    JsonVariant weekdayScheduleHeat_input = doc["weekdayScheduleHeat"];
    if (!weekdayScheduleHeat_input.isNull()) {
      for (int i = 0; i <= MAX_SCHEDULED_EVENTS; i++) {
        weekdayScheduleHeat[i].startHour = doc["weekdayScheduleHeat"][i]["start_hour"];
        weekdayScheduleHeat[i].temp = doc["weekdayScheduleHeat"][i]["temp"];
        i++;
      }
    }

    doc.clear();

    // Serializing data for json output
    doc["mode"] = thermostat_state;
    doc["time"] = now();
    JsonArray temperaturesJson = doc.createNestedArray("temperaturesJson");
    for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
      temperaturesJson.add(temperatures[i]);
    }
    JsonArray weekendScheduleCool_output = doc.createNestedArray("weekendScheduleCool");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      JsonObject temporaryObj = weekendScheduleCool_output.createNestedObject();
      temporaryObj["start_hour"] = weekendScheduleCool[i].startHour;
      temporaryObj["temp"] = weekendScheduleCool[i].temp;
    }
    JsonArray weekdayScheduleCool_output = doc.createNestedArray("weekdayScheduleCool");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      JsonObject temporaryObj = weekdayScheduleCool_output.createNestedObject();
      temporaryObj["start_hour"] = weekdayScheduleCool[i].startHour;
      temporaryObj["temp"] = weekdayScheduleCool[i].temp;
    }
    JsonArray weekendScheduleHeat_output = doc.createNestedArray("weekendScheduleHeat");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      JsonObject temporaryObj = weekendScheduleHeat_output.createNestedObject();
      temporaryObj["start_hour"] = weekendScheduleHeat[i].startHour;
      temporaryObj["temp"] = weekendScheduleHeat[i].temp;
    }
    JsonArray weekdayScheduleHeat_output = doc.createNestedArray("weekdayScheduleHeat");
    for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
      JsonObject temporaryObj = weekdayScheduleHeat_output.createNestedObject();
      temporaryObj["start_hour"] = weekdayScheduleHeat[i].startHour;
      temporaryObj["temp"] = weekdayScheduleHeat[i].temp;
    }

    serializeJson(doc, Serial);
    digitalWrite(COMMUNICATION_LIGHT, LOW);
  }

  // Checking schedule for target temperature
  if (timeStatus() == timeNotSet) {
      digitalWrite(ERROR_LIGHT, HIGH);
  }
  EventStruct *current_schedule;
  switch (thermostat_state)
  {
    case HEAT:
      if ((2 <= weekday()) && (weekday() <= 6)) { // Weekday
        current_schedule = weekdayScheduleHeat;
      } else {
        current_schedule = weekendScheduleHeat;
      }
      break;
    case COOL: 
      if ((2 <= weekday()) && (weekday() <= 6)) { // Weekday
        current_schedule = weekdayScheduleCool;
      } else {
        current_schedule = weekendScheduleCool;
      }
      break;
    default:
      current_schedule = NULL;
      break;
  }
  
  float current_hours = hour() + ((minute() + (second() / 60.)) / 60.);
  int i = 0;
  while (
    current_schedule != NULL &&
    i < MAX_SCHEDULED_EVENTS &&
    current_schedule[i].startHour != NULL &&
    current_hours >= current_schedule[i].startHour
  ) {
    target = current_schedule[i].temp; 
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
