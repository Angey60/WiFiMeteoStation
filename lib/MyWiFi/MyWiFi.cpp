#include "MyWiFi.h"
#include "wifi_constants.h"

MyWiFi::MyWiFi() {}
MyWiFi::~MyWiFi() {}

void MyWiFi::begin()
{
    // Настраиваем WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASS);
}

void MyWiFi::run()
{
}

// Подключение к WiFi точке доступа
bool MyWiFi::connect()
{
    // Если подключение активно, то просто выходим и возвращаем true
    if (!WiFi.isConnected())
    {
        if (WIFI_DEBUG)
        {
            Serial.print(F("Connecting to WiFi AP "));
            Serial.print(SSID);
            Serial.println();
        }

        // Настраиваем WiFi
        begin();

        while (--tries && WiFi.status() != WL_CONNECTED)
        {
            delay(1000);

            if (WIFI_DEBUG)
            {
                Serial.print(F("."));
            }
        }

        Serial.println(F("\r\n"));

        if (WiFi.status() != WL_CONNECTED)
        {
            WiFi.disconnect();

            if (WIFI_DEBUG)
            {
                Serial.println(F("Non Connecting to WiFi ..."));
                Serial.println();
            }
        }
        else
        {
            if (WIFI_DEBUG)
            {
                Serial.print(F("Connecting to WiFi AP "));
                Serial.println(SSID);
                Serial.print(F("IP address: "));
                Serial.println(WiFi.localIP());
                Serial.println();
            }
        }

        return WiFi.isConnected();
    }

    return (WiFi.isConnected());
}

bool MyWiFi::isConnected()
{
    return WiFi.isConnected();
}

void MyWiFi::disconnect()
{
    while (WiFi.status() == WL_CONNECTED)
    {
        WiFi.disconnect();
        delay(100);
    }
}

bool MyWiFi::gpio_status()
{
    return WiFi.isConnected();
}
