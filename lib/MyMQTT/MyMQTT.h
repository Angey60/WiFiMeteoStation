#ifndef MyMQTT_H
#define MyMQTT_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MyMQTT
{

public:
    MyMQTT();
    ~MyMQTT();

    void begin();
    void run();
    void connect();
    //void mqtt_callback(char *topic, byte *payload, unsigned int length);
    
private:
    PubSubClient client;
    
    const char *mqttserver = "mqtt.cloud.yandex.net";
    const char *yandexIoTCoreBrokerId = "a44fnbeb6ejfibpr07j6";
    const char *mqttpassword = "Nz;tkfBytrfpbcnf:bpymRjnfEghjuhfvvbcnf@7389";
    const int mqttport = 8883;
};

#endif // MyMQTT_H