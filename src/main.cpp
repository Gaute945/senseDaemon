#include <WiFi.h> // WiFi library for ESP32

// Replace with your network credentials
const char* ssid = "VLFK-IoT";
const char* password = "jn4GTy11hA";

void setup() {
  Serial.begin(9600);
  
  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Your code here
}
