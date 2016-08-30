/*
Copyright (c) 2013-2016 Ubidots.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Made by Mateo Velez - Metavix for Ubidots Inc

*/
#include "UbidotsMQTT.h"

Ubidots::Ubidots(char* token, void (*callback)(char*,uint8_t*,unsigned int), char* server) {
    _server = server;
    this->callback = callback;
    _token = token;
#if defined(ESP8266)
    _dsName = "ESP8266";
    _pId = "ESP8266";
#else 
    _pId = "MQTT";
    _dsName = "MQTT";
#endif
    currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
    _broker = MQTT(SERVER, MQTT_PORT, callback, client);
}
void Ubidots::setDataSourceLabel(char* dataSourceLabel) {
    _pId = dataSourceLabel;
}
bool Ubidots::connect() {
#if defined (DEBUG_UBIDOTS)
    Serial.println(F("Connecting to MQTT"));
#endif
    return _broker.connect(_pId, _token, NULL);
}
bool Ubidots::getValueSubscribe(char* labelDataSource, char* labelVariable) {
    char topic[250];
    sprintf(topic, "/v1.6/devices/%s/%s/lv", labelDataSource, labelVariable);
    int timeout = 0;
    if (_broker.subscribe(topic)) {
        delay(10);
        return true;
    } else {
        connect();
        delay(10);
        _broker.subscribe(topic);
        delay(10);
        return false;
    }
}
bool Ubidots::sendValues() {
    char topic[100];
    char payload[700];
    uint8_t i = 0;
    String str;
    sprintf(topic, "%s%s", FIRST_PART_TOPIC, _pId);
    sprintf(payload, "{");
    while (i < currentValue) {
        str = String((val+i)->value, 2);
        sprintf(payload, "%s\"%s\":{\"value\":%s", payload, (val+i)->labelName, str.c_str());
        if ((val+i)->timestamp != NULL) {
            sprintf(payload, "%s, \"timestamp\":%s", payload, (val+i)->timestamp);
        }
        if ((val+i)->context != NULL) {
            sprintf(payload, "%s, \"context\":{%s}", payload, (val+i)->context);
        }
        sprintf(payload, "%s}", payload);
        i++;
        if (i >= currentValue) {
            break;
        } else {
            sprintf(payload, "%s, ", payload);
        }
    }
    sprintf(payload, "%s}", payload);
    delay(10);
    Serial.println(payload);
    if (_broker.publish(topic, payload)) {
        currentValue = 0;
        delay(10);
        return true;
    } else {
        connect();
        delay(10);
        _broker.publish(topic, payload);
        delay(10);
        currentValue = 0;
        return false;
    }
}
bool Ubidots::add(char* label, float value) {
    return add(label, value, NULL, NULL);
}
bool Ubidots::add(char* label, float value, char* context) {
    return add(label, value, context, NULL);
}
bool Ubidots::add(char* label, float value, char* context, double timestamp) {
    (val+currentValue)->labelName = label;
    (val+currentValue)->value = value;
    (val+currentValue)->context = context;
    (val+currentValue)->timestamp = timestamp;
    currentValue++;
    if (currentValue > MAX_VALUES) {
        Serial.println(F("You are sending more than the maximum of consecutive variables"));
        currentValue = MAX_VALUES;
    }
}
bool Ubidots::loop() {
    if (_broker.loop()){
        delay(10);
    } else {
        connect();
        delay(10);
        _broker.loop();
        delay(10);
    }
}

// Particular functions
#if defined(ESP8266)
bool Ubidots::wifiConnection(char* ssid, char* pass) {
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    WiFi.setAutoReconnect(true);
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
}
#endif


