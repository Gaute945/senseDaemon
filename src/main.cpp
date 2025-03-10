#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define SENSOR_ADDRESS 1  // Change this if needed

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Starting RS485...");

    if (!ModbusRTUClient.begin(9600)) {
        Serial.println("Failed to start Modbus RTU Client!");
        while (1);
    }
}

void loop() {
    Serial.println("Requesting data...");

    if (ModbusRTUClient.requestFrom(SENSOR_ADDRESS, INPUT_REGISTERS, 0x0000, 1)) {
        Serial.print("Received: ");
        Serial.println(ModbusRTUClient.read());
    } else {
        Serial.println("Failed to read from sensor!");
    }

    delay(1000);
}