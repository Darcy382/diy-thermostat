#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11   

char role = 1;

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

RF24 radio(7, 8); // CE, CSN
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  radio.begin();
  radio.openWritingPipe(address[role]);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();

  dht.begin();
  payload.nodeID = role;
}

void loop() {
  payload.tempurature = dht.readTemperature(true); 
  payload.humidity = dht.readHumidity();  
  payload.heat_idx = dht.computeHeatIndex(payload.tempurature, payload.humidity);

  radio.write(&payload, sizeof(payload));
  delay(1000);
}
