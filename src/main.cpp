#include <ModbusMaster.h>

ModbusMaster node;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial1.begin(9600);         // Sensor default is usually 9600
  node.begin(2, Serial1);      // Sensor default Modbus ID is 2
}

void loop() {
  // Read 3 consecutive registers starting from 0x0000
  uint8_t result = node.readHoldingRegisters(0x0000, 3);

  if (result == node.ku8MBSuccess) {
    uint16_t reg0 = node.getResponseBuffer(0);

    Serial.print("Reg 0x0000 Wind speed): ");
    Serial.print(reg0);
    Serial.print(" → ");
    Serial.print(reg0 / 10.0);
    Serial.println(" m/s");
    Serial.println("---");

  } else {
    Serial.print("Modbus read failed. Error code: ");
    Serial.println(result);
  }

  delay(1000);
}