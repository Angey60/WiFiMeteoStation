#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>
#include "wifi_constants.h"

class WiFiClass
{
public:
    byte tries = 50; // Попыткок подключения к точке доступа
    
    void begin()
    {
        // Настраиваем WiFi
        WiFi.mode(WIFI_STA);
        WiFi.begin(SSID, PASS);

        while (--tries && WiFi.status() != WL_CONNECTED)
        {
            delay(250);
            Serial.print(F("."));
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println();
            Serial.println(F("Non Connecting to WiFi.."));
        }
        else
        {
            Serial.println();
            Serial.print(F("Connecting to WiFi AP "));
            Serial.println(SSID);
            Serial.print(F("IP address: "));
            Serial.println(WiFi.localIP());
        }
    }

    bool isConnected()
    {
        return WiFi.isConnected();
    }

    void disconnect()
    {
        if (WiFi.isConnected())
        {
            WiFi.disconnect();
        };
    }

protected:
private:
    
};