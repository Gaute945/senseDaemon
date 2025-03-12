#include <Arduino.h>
#include <Wire.h>
#include "DHT.h"

#define LED_PIN 4
#define DHT11_PIN 7

DHT dht11(DHT11_PIN, DHT11);

float humidity = 0.0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  dht11.begin();

  pinMode(LED_PIN, OUTPUT);
}

void printSensors() {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(F(" %"));
}

void loop() {
  // dht11
  humidity = dht11.readHumidity();

  printSensors();  
  delay(1000);
}
