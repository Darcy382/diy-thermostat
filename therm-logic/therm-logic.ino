#define FAN_PIN 10
#define AC_PIN 11
#define HEATER_PIN 12

enum Thermostat_state_name { HEAT, COOL, FAN, THERM_OFF };
enum Power_state { OFF, ON };
const float bound = 1;

int thermostat_state = THERM_OFF;
float target;
float current_temp;

void setup() {
  Serial.begin(9600);
  thermostat_state = OFF;
  target = 72;
}

void loop() {
  current_temp = 72;

  // TODO: Turn the fan off when fan mode exited
  switch(thermostat_state) {
    case HEAT :
      if (ac_on()) {
        turn_ac(OFF);
      }
      
      // Turn heater on
      if (!heater_on() && current_temp < target - bound) {
        turn_fan(ON);
        turn_heater(ON);
      } 
      // Turn heater off
      else if (heater_on() && current_temp > target + bound) {
        turn_fan(OFF);
        turn_heater(OFF);
      }
      break;
    case COOL :
      if (heater_on()) {
        turn_heater(OFF);
      }

      // Turn ac on
      if (!ac_on() && current_temp > target + bound) {
        turn_fan(ON);
        turn_ac(ON);
      }
      // Turn ac off
      else if (ac_on() && current_temp < target - bound) {
        turn_fan(OFF);
        turn_ac(OFF);
      }
      break;
    case FAN :
      turn_fan(ON);
      
      if (heater_on()) {
        turn_heater(OFF);
      }
      if (ac_on()) {
        turn_ac(OFF);
      }
      break;
    case THERM_OFF :
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
      Serial.print("ERROR: Invalid state.  state = ");
      Serial.println(thermostat_state);
  }
  delay(1000);
}

void turn_fan(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(FAN_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(FAN_PIN, LOW);
  } else {
    Serial.println("Invalid power state");
  }
}

void turn_heater(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(HEATER_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(HEATER_PIN, LOW);
  } else {
    Serial.println("Invalid power state");
  }
}

void turn_ac(Power_state power_state) {
  if (power_state == ON) {
    digitalWrite(AC_PIN, HIGH);
  } else if (power_state == OFF) {
    digitalWrite(AC_PIN, LOW);
  } else {
    Serial.println("Invalid power state");
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
