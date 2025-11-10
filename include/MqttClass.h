//#include <Arduino.h>
//#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MqttClass {

    public:
        // Статус метеостанции
        char status = 0x0;
    

    private:
        // Указатель на MQTT-клиента
        PubSubClient client;
};