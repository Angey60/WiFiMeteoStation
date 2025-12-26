#include "MyWiFi.h"
#include <ArduinoJson.h>
#include "wifi_constants.h"

// JsonDocument doc;
// char json_buffer[50] = {0};
// char json_buffer_err[50] = {0};

MyWiFi::MyWiFi() {}
MyWiFi::~MyWiFi() {}

void MyWiFi::begin(const char *SSID, const char *PASS)
{
    ssid = SSID;
    pass = PASS;

    // Настраиваем WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
}

const char *MyWiFi::ssid_def()
{
    return _SSID;
}

const char *MyWiFi::pass_def()
{
    return _PASS;
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
            Serial.print(ssid);
            Serial.println();
        }

        // Настраиваем WiFi
        begin(ssid, pass);

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
                Serial.println(ssid);
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
        custom_delay(100);
    }
}

bool MyWiFi::gpio_status()
{
    return WiFi.isConnected();
}
