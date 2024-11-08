#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Create an instance of the sensor
Adafruit_BMP280 bmp; // I2C interface by default

void setup() {
  Serial.begin(9600);
  
  // Initialize the BMP280
  if (!bmp.begin(0x77)) {  // Check the I2C address (0x76 or 0x77)
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Optional: set the precision and mode
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, 
                  Adafruit_BMP280::SAMPLING_X2,  // Temperature oversampling
                  Adafruit_BMP280::SAMPLING_X16, // Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,   // Filtering
                  Adafruit_BMP280::STANDBY_MS_500); // Standby time
}

void loop() {
  Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" °C");

  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure() / 100.0F); // Convert to hPa
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(1021)); // Set baseline sea-level pressure for accuracy
  Serial.println(" m");

  Serial.println();
  delay(2000);
}
