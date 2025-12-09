#ifndef MyOTA_H
#define MyOTA_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266httpUpdate.h>
#include <CustomClass.h>

class MyOTA : public CustomClass
{
public:
    MyOTA();
    ~MyOTA();

    void begin();

protected:
private:
};

#endif // MyOTA_H