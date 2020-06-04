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

#ifndef UbidotsMQTT_H
#define UbidotsMQTT_H
#include "PubSubClient.h"

#if defined(ESP8266)
#include "ESP8266WiFi.h"
#endif

#define MQTT_PORT 1883
#define SERVER "industrial.api.ubidots.com"
#define MAX_VALUES 5
#define FIRST_PART_TOPIC "/v1.6/devices/"

typedef struct Value {
    char  *labelName;
    char  *context;
    double timestamp;
    float value;
} Value;


class Ubidots {
 private:
    void (*callback)(char*,uint8_t*,unsigned int);
    PubSubClient _broker;
#if defined(ESP8266)
    WiFiClient client;
#endif
    uint8_t currentValue;
    char* _server;
    char* _token;
    char* _pId;
    char* _dsName;
    Value * val;

 public:
    Ubidots(char* token, void (*callback)(char*,uint8_t*,unsigned int), char* server = SERVER);
    bool loop();
    void setDataSourceLabel(char* dataSourceLabel);
    bool sendValues();
    bool getValueSubscribe(char* labelDataSource, char* labelVariable);
    bool connect();
    bool add(char* label, float value);
    bool add(char* label, float value, char* context);
    bool add(char* label, float value, char* context, double timestam);
//  Particular functions    
#if defined(ESP8266)
    bool wifiConnection(char* ssid, char* pass);
#endif
};

#endif
