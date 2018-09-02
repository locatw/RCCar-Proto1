#include <ESP8266WiFi.h>
#include "Secrets.h"

#define SERVER_PORT 54237

WiFiServer server(SERVER_PORT);
WiFiClient client;
boolean client_connected = false;

void process();

void setup() {
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
            Serial.println("Command: command1");
            break;
        case 0x2:
            Serial.println("Command: command2");
            break;
        default:
            break;
    }
}