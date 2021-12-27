/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

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

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address[0]);
  radio.openReadingPipe(1, address[1]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));
//    Serial.print("Temp: ");
//    Serial.print(data[0]);
//    Serial.write(0xC2);  //send degree symbol
//    Serial.write(0xB0);  //send degree symbol
//    Serial.print("F   ");
//    Serial.print("Humidity: ");
//    Serial.print(data[1]); 
//    Serial.print("%   ");
//    Serial.print("Heat Index: ");
//    Serial.print(data[2]);
//    Serial.write(0xC2);  //send degree symbol
//    Serial.write(0xB0);  //send degree symbol
//    Serial.println("F   ");
      Serial.println(data[0]);
  }
}
