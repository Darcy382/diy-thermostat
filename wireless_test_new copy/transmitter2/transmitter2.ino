/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11   

RF24 radio(7, 8); // CE, CSN

uint64_t address[6] = {
  0x7878787878LL,
  0xB3B4B5B6F1LL,
  0xB3B4B5B6CDLL,
  0xB3B4B5B6A3LL,
  0xB3B4B5B60FLL,
  0xB3B4B5B605LL
};
float data[3];

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  radio.begin();
  radio.openWritingPipe(address[1]);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  dht.begin();
}

void loop() {
  float tempurature = dht.readTemperature(true); 
  float humidity = dht.readHumidity();  
  float heat_idx = dht.computeHeatIndex(tempurature, humidity);
  
  data[0] = tempurature;
  data[1] = humidity;
  data[2] = heat_idx;
  radio.write(&data, sizeof(data));
  delay(1000);
}
