#include <Arduino.h>

// Define the analog pin connected to the GUVA-S12SD
const int uvSensorPin = A0;  // Use GPIO 34 or any other ADC pin on the ESP32

const float calibrationFactor = 307.0; // Approximate μW/cm² per V for GUVA-S12SD

void setup() {
  Serial.begin(9600);  // ESP32 default is 115200 baud
}

void loop() {
  // Read the analog value from the sensor (0-4095 for 3.3V on ESP32)
  int sensorValue = analogRead(uvSensorPin);
  digitalWrite(4, HIGH);

  // Convert the analog value to voltage
  float sensorVoltage = sensorValue * (3.3 / 4095.0);  // ESP32 ADC is 12-bit (0-4095)

  // Calculate UV intensity in μW/cm²
  float uvIntensity = sensorVoltage * calibrationFactor;

  // Print the results
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(sensorVoltage, 2);
  Serial.print(" V | UV Intensity: ");
  Serial.print(uvIntensity, 2);
  Serial.println(" μW/cm²");

  delay(1000);  // 1 second delay
  digitalWrite(4, LOW);
}