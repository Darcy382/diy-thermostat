#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ArduinoJson.h>

#define FAN_PIN 4
#define AC_PIN 3
#define HEATER_PIN 2
#define COMMUNICATION_LIGHT 5
#define RADIO_LIGHT 14
#define HEAT_MODE_LIGHT 6
#define COOL_MODE_LIGHT 9
#define FAN_MODE_LIGHT 10
#define NUM_TEMP_SENSORS 2

enum Thermostat_state_name { THERM_OFF=0, HEAT=1, COOL=2, FAN=3 };
enum Power_state { OFF, ON };
const float bound = 1; // Convert to define?

RF24 radio(7, 8); // CE, CSN
StaticJsonDocument<200> doc;

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
  float tempurature;
  float humidity;
  float heat_idx;
};
PayloadStruct payload;


int thermostat_state;
float target;
float avg_temp;
float tempuratures[NUM_TEMP_SENSORS];
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
  
  radio.begin();
  for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
    radio.openReadingPipe(i, address[i]);
  }
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {

  for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
    tempuratures[i] = NULL;
  }

  
  // Recieving tempuratures from all sensors
  digitalWrite(RADIO_LIGHT, HIGH);
  received_all_temps = false;
  while (!received_all_temps) {
    if (radio.available()) {
      radio.read(&payload, sizeof(payload));
      if (0 <= payload.nodeID <= NUM_TEMP_SENSORS - 1) {
        tempuratures[payload.nodeID] = payload.tempurature;
      } else {
        // TODO: Turn on the error light
        // An invalid nodeID has been recived
      }
    }

    received_all_temps = true;
    for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
      if (tempuratures[i] == NULL) {
        received_all_temps = false;
      }
    }
  }
  digitalWrite(RADIO_LIGHT, LOW);
  
  // Checking for computer input
  if (Serial.available() != 0) {
    digitalWrite(COMMUNICATION_LIGHT, HIGH);
    // Deserializing computer json input
    String input = Serial.readString();
    DeserializationError error = deserializeJson(doc, input);
    if (error) {
      // Turn on the Error light
      // Serial.print(F("deserializeJson() failed: "));
      // Serial.println(error.f_str());
      return;
    }

    JsonVariant mode = doc["mode"];
    if (!mode.isNull()) {
      thermostat_state = doc["mode"];
    }
    doc.clear();

    // Serializing data for json output
    doc["mode"] = thermostat_state;
    JsonArray tempuraturesJson = doc.createNestedArray("tempuraturesJson");
    for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
      tempuraturesJson.add(tempuratures[i]);  
    }
    serializeJson(doc, Serial);
    digitalWrite(COMMUNICATION_LIGHT, LOW);
  }

  // Calculating average tempurature
  float sum = 0;
  for (int i = 0; i < NUM_TEMP_SENSORS; i++) {
    sum += tempuratures[i];
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
      // Turn on the error light
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
