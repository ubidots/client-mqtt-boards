#include "UbidotsMQTT.h"

#define WIFISSID "My_SSID"
#define PASSWORD "password123"
#define TOKEN "xxxx"

void callback(char* topic, byte* payload, unsigned int length);

Ubidots client(TOKEN, callback);

void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
    Serial.write(payload, length);
    Serial.println(topic);
}
void setup() {
    Serial.begin(115200);
    delay(10000);
    Serial.println("Connecting WiFi");
    client.wifiConnection(WIFISSID, PASSWORD);
    Serial.println("Connected");
    while (client.connect());
}


void loop() {
    client.add("my-device" ,"led");
    client.loop();
}


