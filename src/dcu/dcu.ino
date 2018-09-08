#include <Wire.h>

#define DEVICE_ID_DCU 1

void i2c_receive_event(int num_bytes);

void setup() {
    Serial.begin(115200);

    Wire.begin(DEVICE_ID_DCU);
    Wire.onReceive(i2c_receive_event);
}

void loop() {
    delay(100);
}

void i2c_receive_event(int num_bytes) {
    int command = Wire.read();
    Serial.println(command);

    int value = Wire.read();
    Serial.println(value);
}