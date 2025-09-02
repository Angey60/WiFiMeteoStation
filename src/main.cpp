#include <Arduino.h>
#include <avr/pgmspace.h>
#include <time.h>

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266httpUpdate.h>

#include <ArduinoJson.h>
#include <Adafruit_I2CDevice.h>
// библиотека для работы с модулем Slot Expander (I²C IO)
#include <GpioExpander.h>
// создаём объект expander класса GpioExpander по адресу 42
GpioExpander expander(43);

#include <CERTIFICATES.h>
#include <constants.h>
#include <service_functions.h>
#include <OTA.h>
#include <NET.h>
#include <MQTT.h>

void setup()
{
  // put your setup code here, to run once:
  Wire.begin();
  // Инициализируем объект expander.
  expander.begin();
  // Инициализируем индикаторы
  pinMode(gpioRelay, OUTPUT);
  expander.pinMode(expander_gpioRelay, OUTPUT);

  DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
  while (!DEBUG_SERIAL)
  {
    ;
  }
  delay(500);

  if (DEBUG)
  {
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println(F("Device started ..."));
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.flush();
    delay(500);
  }

  if (DEBUG)
  {
    DEBUG_SERIAL.println(F("Meteo Sensor init OK"));
    DEBUG_SERIAL.println(F("Demo project FOR ESP8266"));
    DEBUG_SERIAL.flush();
  }

  // Индикатор включения/отклячения метеостанции
  expander.digitalWrite(expander_gpioRelay, lvlRelayOn);
  delay(500);
  expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
  delay(500);

  // Привязываем корневой сертификат к клиенту Iot Core
  mqttServ.setTrustAnchors(&mqttCert);
  
  // Настраиваем клиент Iot Core
  mqtt_client.setServer(mqttserver, mqttport);
  mqtt_client.setCallback(callback);
  mqtt_client.setBufferSize(1024);
  mqtt_client.setKeepAlive(15);

  // Подключаемся к WiFi
  if (wifiConnected())
  {
    // Корректируем дату и время
    setClock();
    // Подключаемся к Iot Core
    connect();
    
    // Тестируем OTA
    delay(5000);
    otaStart(firmware_url.c_str());
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  mqtt_client.loop();
  // mqtt_flag = 0x00;
  if (!mqtt_client.connected())
  {
    // Корректируем дату и время
    setClock();
    // Подключаемся к Iot Core
    connect();
  }
}
