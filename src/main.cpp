#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266httpUpdate.h>
#include <LittleFS.h>
#include <time.h>
#include <ArduinoJson.h>
#include <Adafruit_I2CDevice.h>
// библиотека для работы с модулем Slot Expander (I²C IO)
#include <GpioExpander.h>

#include <CERTIFICATES.h>
#include <constants.h>
#include <service_functions.h>
#include <OTA.h>
#include <NET.h>
#include <MQTT.h>

void setup()
{
  // put your setup code here, to run once:
  // Инициализируем объект expander. Для Arduino Due - adio.begin(&Wire1);
  expander.begin();
  
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

  DEBUG_SERIAL.println(F("Meteo Sensor init OK"));
  DEBUG_SERIAL.println(F("Demo project FOR ESP8266"));
  DEBUG_SERIAL.flush();

  pinMode(gpioRelay, OUTPUT);
  expander.pinMode(expander_gpioRelay, OUTPUT);

  expander.digitalWrite(expander_gpioRelay, lvlRelayOn);
  delay(1000);
  expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
  delay(500);

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
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  mqtt_client.loop();
  // mqtt_flag = 0x00;
  if (!mqtt_client.connected())
  {
    connect();
  }
}
