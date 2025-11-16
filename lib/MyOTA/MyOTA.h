#ifndef MyOTA_H
#define MyOTA_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266httpUpdate.h>

class MyOTA
{
public:
    MyOTA();
    ~MyOTA();

    void begin();
    void run();

protected:
private:
};

#endif // MyOTA_H