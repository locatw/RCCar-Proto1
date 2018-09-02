#include <Arduino.h>
#include <Wire.h>

#define DEVICE_ID 8

void receiveEvent(int numBytes);

void setup() {
    Serial.begin(115200);

    Wire.begin(DEVICE_ID);
    Wire.onReceive(receiveEvent);
}

void loop() {
    delay(100);
}

void receiveEvent(int numBytes) {
    const uint8_t command = (uint8_t)Wire.read();
    switch (command) {
        case 0x1:
            Serial.println("[Slave] Command: command1");
            break;
        case 0x2:
            Serial.println("[Slave] Command: command2");
            break;
        default:
            break;
    }
}