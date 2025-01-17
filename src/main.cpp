#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

#define RAIN_SENSOR_PIN A0
#define LED_PIN 2
#define DHT11_PIN D2
#define SCK_PIN A5 // I2C Clock
#define SDI_PIN A4 // I2C Data

Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(SDI_PIN, SCK_PIN); // Set custom I2C pins
  bmp.begin(0x77);
}

void loop() {
  int rainSensor = analogRead(RAIN_SENSOR_PIN);
  Serial.print("Rain Sensor Value: ");
  Serial.println(rainSensor);

  if (rainSensor < 3500) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  
  // dht11
  // BMP280
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100;
  float altitude = bmp.readAltitude(1019.66);

  if (!isnan(temperature) && !isnan(pressure) && !isnan(altitude)) {
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(F(" *C"));

    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(F(" hPa"));

  } else {
    Serial.println(F("Error reading from BMP280 sensor!"));
  }

  // UV
  // wind speed
  
  delay(1000); // Wait 1 second before repeating
}