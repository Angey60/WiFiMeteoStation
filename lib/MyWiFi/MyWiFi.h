#ifndef MyWiFi_H
#define MyWiFi_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>
#include <CustomClass.h>

class MyWiFi : public CustomClass
{
public:
    MyWiFi();
    ~MyWiFi();
    void begin();

    //  Подключение к WiFi точке доступа
    bool connect();
    bool isConnected();
    bool gpio_status();
    void disconnect();

private:
    byte tries = 15; // Попыткок подключения к точке доступа
};

#endif // MyWiFi_H
