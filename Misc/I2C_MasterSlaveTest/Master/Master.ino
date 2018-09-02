#include <ESP8266WiFi.h>
#include <Wire.h>
#include "Secrets.h"

#define SERVER_PORT 54237
#define SLAVE_DEVICE_ID 8

WiFiServer server(SERVER_PORT);
WiFiClient client;
boolean client_connected = false;

void process();
void send_command(int command);

void setup() {
    Wire.begin();

    Serial.begin(115200);

    Serial.print("Connect to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Success");

    server.begin();

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    process();

    delay(100);
}

void process() {
    if (!client_connected) {
        client = server.available();
        delay(1);

        if (client.connected()) {
            client_connected = true;
        }
        else {
            return;
        }
    }

    if (!client.connected()) {
        client_connected = false;
        return;
    }

    const int data = (uint)client.read();
    if (data == -1) {
        return;
    }

    const uint command = (uint)data;
    switch (command) {
        case 0x1:
            Serial.println("[Master] Command: command1");
            send_command(command);
            break;
        case 0x2:
            Serial.println("[Master] Command: command2");
            send_command(command);
            break;
        default:
            break;
    }
}

void send_command(int command) {
    Wire.beginTransmission(SLAVE_DEVICE_ID);
    Wire.write(command);
    Wire.endTransmission();
}