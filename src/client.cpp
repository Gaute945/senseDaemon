#include <NimBLEDevice.h>

// Callback class to handle discovered devices
class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) override {
        Serial.printf("Advertised Device found: %s\n", advertisedDevice->toString().c_str());
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting NimBLE Client");

    // Initialize NimBLE
    NimBLEDevice::init("");

    // Retrieve the scan object
    NimBLEScan* pScan = NimBLEDevice::getScan();

    // Set the callback for discovered devices
    pScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

    // Set scan parameters
    pScan->setInterval(45);        // Time between scans (in 0.625ms units)
    pScan->setWindow(15);          // Duration to scan (in 0.625ms units)
    pScan->setActiveScan(true);    // Active scan requests scan response data

    // Start scanning indefinitely (0 = forever)
    pScan->start(0, nullptr, false);
}

void loop() {
    // Perform other tasks here
    delay(1000);
}
