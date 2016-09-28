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
    _broker = PubSubClient(client);
}

void Ubidots::setDataSourceLabel(char* dataSourceLabel) {
    _pId = dataSourceLabel;
}

bool Ubidots::setServerAndCallback() {
    _broker.setServer(SERVER, MQTT_PORT);
    _broker.setCallback(callback);
    return true;
}

bool Ubidots::getValueSubscribe(char* labelDataSource, char* labelVariable) {
    char topic[250];
    sprintf(topic, "/v1.6/devices/%s/%s/lv", labelDataSource, labelVariable);
    while (!_broker.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (_broker.connect("ESP8266Client", "i8RINXyaCy0gPNC52WPwFdTkFZnzMt", NULL)) {
            Serial.println("connected");
            _broker.subscribe(topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(_broker.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
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
    while (!_broker.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (_broker.connect("ESP8266Client")) {
              Serial.println("connected");
              _broker.publish(topic, payload);
        } else {
              Serial.print("failed, rc=");
              Serial.print(_broker.state());
              Serial.println(" try again in 5 seconds");
              delay(5000);
        }
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
   return _broker.loop();
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


