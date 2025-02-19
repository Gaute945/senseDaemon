#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include "DHT.h"

#define RAIN_PIN A0
#define LED_PIN D4
#define SCK_PIN A5 // I2C Clock
#define SDI_PIN A4 // I2C Data
#define DHT11_PIN D7
#define UV_PIN A3

Adafruit_BMP280 bmp;
DHT dht11(DHT11_PIN, DHT11);

float temperature = 0.0;
float pressure = 0.0;
float humidity = 0.0;
int rainSensor = 0;
int uvReading = 0;
bool rain = false;
const float REF_VOLTAGE = 3.3; // 3.3V
const int ADC_RESOLUTION = 4095; // 12-bit ADC (ESP32)
const float UV_SENSITIVITY = 0.1; // 0.1 V per (mW/cm²)
float uvIndex = 0.0;

void setup() {
  Serial.begin(9600);
  Wire.begin(SDI_PIN, SCK_PIN);
  bmp.begin(0x77);
  dht11.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(UV_PIN, INPUT);
}

void printSensors() {
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(F(" *C"));
    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(F(" hPa"));

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(F(" %"));

    Serial.print("Rain Sensor Value: ");
    Serial.println(rainSensor);
    Serial.print("Is it raining? ");
    Serial.println(rain);

    Serial.print("UV Reading: ");
    Serial.println(uvReading);
    Serial.print("UV Index: ");
    Serial.println(uvIndex);
    Serial.print("\n");
    Serial.print("\n");
}

float calculateUVIndex() {
  int rawValue = analogRead(UV_PIN); 
  float voltage = (rawValue * REF_VOLTAGE) / ADC_RESOLUTION;
  float uvIntensity = voltage / UV_SENSITIVITY; // Convert to mW/cm²
  float uvIndex = uvIntensity * 0.1; // Approximate conversion to UV Index
    
  return uvIndex;
}

void loop() {
  // diagnose light
  if (rainSensor < 3500) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  
  // rain sensor
  rainSensor = analogRead(RAIN_PIN);
  rain = (rainSensor < 3500);
  
  // dht11
  humidity = dht11.readHumidity();
  
  // BMP280
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure() / 100;

  // UV
  uvReading = analogRead(UV_PIN);
  uvIndex = calculateUVIndex(); // Update UV index

  printSensors();  
  delay(1000);
}