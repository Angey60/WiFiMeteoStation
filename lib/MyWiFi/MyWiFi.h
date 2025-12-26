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

    void begin(const char *SSID, const char *PASS);
    //  Подключение к WiFi точке доступа
    bool connect();
    bool isConnected();
    bool gpio_status();
    void disconnect();
    // По умолчанию
    const char *ssid_def();
    const char *pass_def();

private:
    const char *ssid;
    const char *pass;

    byte tries = 15; // Попыткок подключения к точке доступа
};

#endif // MyWiFi_H
