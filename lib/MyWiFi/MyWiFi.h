#ifndef MyWiFi_H
#define MyWiFi_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>

class MyWiFi
{
public:
    MyWiFi();
    ~MyWiFi();
    void begin();
    void run();

    //  Подключение к WiFi точке доступа
    bool connect();
    bool isConnected();
    void disconnect();

private:
    byte tries = 15; // Попыткок подключения к точке доступа
};

#endif // MyWiFi_H
