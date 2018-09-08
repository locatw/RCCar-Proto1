#include <ESP8266WiFi.h>
#include <Wire.h>
#include "secrets.h"
#include "../../../../common/device.h"

#define DEVICE_ID_DCU 1

#define CMD_KIND_STEERING_ANGLE 1

#define SERVER_PORT 54237

struct Command {
    int8_t kind;
    int8_t value;
};

WiFiServer server(SERVER_PORT);
WiFiClient client;
boolean client_connected = false;

void init_wifi();
Command* read_command();
void send_command(Command*);

void setup() {
    Serial.begin(115200);

    init_wifi();

    Wire.begin();
}

void loop(){
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

    Command* command = read_command();
    if (command != NULL) {
        Serial.println(command->kind);
        Serial.println(command->value);

        send_command(command);

        delete command;
    }
}

void init_wifi() {
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

Command* read_command() {
    const size_t buffer_size = 2;
    uint8_t buffer[buffer_size] = {};

    int read_count = 0;
    while ((read_count = client.readBytes(buffer, buffer_size)) != buffer_size) { }
    
    Command* command = new Command();
    command->kind = (int8_t)buffer[0];
    command->value = (int8_t)buffer[1];

    return command;
}

void send_command(Command* command) {
    if (command == NULL) { return; }

    Wire.beginTransmission(DEVICE_ID_DCU);
    Wire.write(command->kind);
    Wire.write(command->value);
    Wire.endTransmission();
}