#ifndef MyMQTT_H
#define MyMQTT_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <CustomClass.h>

class MyMQTT : public CustomClass
{
public:
    MyMQTT();
    ~MyMQTT();

    void begin();

    bool connect();
    bool isConnected();
    bool gpio_status();
    bool loop();

    void disConnect();
    void publish(const char *json_buffer);
    void setCallback(std::function<void(char *, uint8_t *, unsigned int)> callback);

    // Статус метеостанции
    char status = 0x0;

protected:
private:
};

#endif // MyMQTT_H