#include <Arduino.h>
#include <NimBLEDevice.h>

// Define custom server callbacks
class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer) {
    Serial.println("Client connected");
  }

  void onDisconnect(NimBLEServer* pServer) {
    Serial.println("Client disconnected");
    // Restart advertising upon disconnection
    NimBLEDevice::startAdvertising();
  }

  void onAdvertisingStart(NimBLEAdvertising* pAdvertising) {
    Serial.println("Advertising started");
  }
};

NimBLEServer* pServer;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting NimBLE Server");

  // Initialize NimBLE with a device name
  NimBLEDevice::init("NimBLE");

  // Create the server and assign the custom callbacks
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create a service (named "DEAD") and add a characteristic ("BEEF")
  NimBLEService* pService = pServer->createService("DEAD");
  NimBLECharacteristic* pCharacteristic = pService->createCharacteristic(
    "BEEF",
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE |
    NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC
  );
  pCharacteristic->setValue("Burger");
  pService->start();

  // Set up advertising: include the service UUID and set the device name
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->setName("NimBLE-Server");
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->enableScanResponse(true);
  pAdvertising->start();

  Serial.println("Advertising Started");
}

void loop() {
  delay(2000);
}