#ifndef MyWiFi_H
#define MyWiFi_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>

//include <constants.h>

class MyWiFi
{
public:
    MyWiFi();
    ~MyWiFi();
    void begin();
    void run();

protected:
    // Подключение к WiFi точке доступа
    bool connect();
    bool isConnected();
    void disconnect();
    
private:
    //std::function<void(char*, uint8_t*, unsigned int)> cc;
    const char *SSID = "MTSRouter_B769";
    const char *PASS = "98797899";
};

#endif // MyWiFi_H
