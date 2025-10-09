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

    bool connect();
    bool isConnected();
    void disconnect();
    bool gpio_status();

private:
    PubSubClient client;
};

#endif // MyMQTT_H