#include <Arduino.h>

#define RAIN_SENSOR_PIN A0  // Analog pin connected to the rain sensor
#define LED_PIN 13          // Optional: an LED for indicating rain detection

void setup() {
  Serial.begin(9600);       // Start serial communication
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
}

void loop() {
  int sensorValue = analogRead(RAIN_SENSOR_PIN); // Read analog value from the rain sensor
  Serial.print("Rain Sensor Value: ");
  Serial.println(sensorValue);

  if (sensorValue < 500) {      // Adjust threshold based on your testing
    digitalWrite(LED_PIN, HIGH); // Turn on LED if rain detected
  } else {
    digitalWrite(LED_PIN, LOW);  // Turn off LED otherwise
  }

  delay(1000); // Wait 1 second before repeating
}