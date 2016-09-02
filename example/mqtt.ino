#include "UbidotsMQTT.h"

#define WIFISSID "UBIWIFI"
#define PASSWORD "clave123456789ubi"

void callback(char* topic, byte* payload, unsigned int length);

Ubidots client("xsK1D8TN97ip0WFemG6kc6PYIWFJOU", callback);

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
  Serial.println("looooop");
    client.getValueSubscribe("demo-ruta-n" ,"lampara");
    client.loop();
}


