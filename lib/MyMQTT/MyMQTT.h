#ifndef MyMQTT_H
#define MyMQTT_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MyMQTT
{
public:
    MyMQTT();
    ~MyMQTT();
    void begin();
    void run();

    bool connect();
    bool isConnected();
    bool gpio_status();
    bool loop();

    void disConnect();
    void publish(const char *json_buffer);

    // Статус метеостанции
    char status = 0x0;
    // Указатель на MQTT-клиента
    PubSubClient client;

protected:
    
private:
    //std::function<void(char*, uint8_t*, unsigned int)> cc;
    void cc(char*, uint8_t*, unsigned int);
};

#endif // MyMQTT_H