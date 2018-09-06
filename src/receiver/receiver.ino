#include <ESP8266WiFi.h>
#include "secrets.h"

#define SERVER_PORT 54237

WiFiServer server(SERVER_PORT);
WiFiClient client;
boolean client_connected = false;

void init_wifi();

void setup() {
    Serial.begin(115200);

    init_wifi();
}

void loop(){

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