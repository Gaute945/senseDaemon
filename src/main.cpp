#include <Arduino.h>
#include <Arduino_MKRENV.h>

#define RAIN_PIN A0
#define LED_PIN 4
#define UV_PIN A3

float temperature = 0.0;
float pressure = 0.0;
float humidity = 0.0;
int rainSensor = 0;
int uvReading = 0;
bool rain = false;
const float REF_VOLTAGE = 3.3; // 3.3V
const float UV_SENSITIVITY = 0.1; // 0.1 V per (mW/cm²)
float uvIndex = 0.0;

void setup() {
  Serial.begin(9600);
  ENV.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(UV_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
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
  float voltage = (rawValue * REF_VOLTAGE) / 1023.0;
  float uvIntensity = voltage / UV_SENSITIVITY; // Convert to mW/cm²
  float uvIndex = uvIntensity * 0.1; // Approximate conversion to UV Index
  return uvIndex;
}

void loop() {
  // rain sensor
  rainSensor = analogRead(RAIN_PIN);
  rain = (rainSensor < 1000);
  
  // shield
  humidity = ENV.readHumidity();
  temperature = ENV.readTemperature(CELSIUS);
  pressure = ENV.readPressure();

  // UV
  uvReading = analogRead(UV_PIN);
  uvIndex = calculateUVIndex(); // Update UV index

  printSensors();  
  delay(1000);
}
