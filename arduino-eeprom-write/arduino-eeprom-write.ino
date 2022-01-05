#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  while (!Serial) {;}
  
  int eeAddress = 0;
  float ary[4] = {0, 0, 0, 0};
  eeAddress = writeList(eeAddress, ary);
  eeAddress = writeList(eeAddress, ary);
  eeAddress = writeList(eeAddress, ary);
  eeAddress = writeList(eeAddress, ary);
  eeAddress = writeList(eeAddress, ary);
  eeAddress = writeList(eeAddress, ary);
  eeAddress = writeList(eeAddress, ary);
  eeAddress = writeList(eeAddress, ary);
  Serial.println("Writing complete!");
}

void loop() {
  /* Empty loop */
}


int writeList(int eeAddress, float ary[4]) {
  int incr = sizeof(float);
  for (int i = 0; i < 4; i++) {
    EEPROM.put(eeAddress, ary[i]);
    eeAddress+=incr;
  }
  return eeAddress;
}

int readList(int eeAddress, float ary[4]) {
  int incr = sizeof(float);
  for (int i = 0; i < 4; i++) {
    EEPROM.get(eeAddress, ary[i]);
    eeAddress+=incr;
  }
  return eeAddress;
}
