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

protected:
    
private:
    //std::function<void(char*, uint8_t*, unsigned int)> cc;
};

#endif // MyWiFi_H
