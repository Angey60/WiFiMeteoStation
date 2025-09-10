#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <time.h>
//
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//
#include <Adafruit_I2CDevice.h>
// библиотека для работы с модулем Slot Expander (I²C IO)
#include <GpioExpander.h>
// создаём объект expander класса GpioExpander по адресу 42
GpioExpander expander(43);
#include <Adafruit_BusIO_Register.h>
#include <TroykaIMU.h>
#include <QuadDisplay2.h>
// создаём объект класса QuadDisplay и передаём номер пина CS
QuadDisplay qd(4);
#include <CERTIFICATES.h>
#include <constants.h>
#include <service_functions.h>
#include <OTA.h>
#include <NET.h>
#include <MQTT.h>
#include <LITTLEFS1.h>

void setup()
{
  // put your setup code here, to run once:
  Wire.begin();
  // Инициализируем бортовые часы
  initClock();
  //  Инициализируем объект display
  qd.begin();
  // Инициализируем объект expander.
  expander.begin();
  // Инициализируем индикаторы
  // красная лампочка On/Off WiFi
  pinMode(gpioWiFi, OUTPUT);
  // зеленая лампочка On/Off метеостанции
  expander.pinMode(gpioOnOff, OUTPUT);
  expander.digitalWrite(gpioOnOff, lvlRelayOff);
  // синяя лампочка On/Off MQTT !!!
  expander.pinMode(gpioMQTT, OUTPUT);
  expander.digitalWrite(gpioMQTT, lvlRelayOff);
  // Инициализируем метеостанцию
  SHT3x.begin();

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
  expander.digitalWrite(gpioOnOff, lvlRelayOn);
  delay(500);
  expander.digitalWrite(gpioOnOff, lvlRelayOff);
  delay(500);

  // Индикатор включения/отклячения метеостанции
  expander.digitalWrite(gpioMQTT, lvlRelayOn);
  delay(500);
  expander.digitalWrite(gpioMQTT, lvlRelayOff);
  delay(500);

  if (DEBUG)
  {
    DEBUG_SERIAL.println(F("\r\n"));
  }

  // Создаем слиент Yandex Iot Core
  MQTClient();

  // Подключаемся к WiFi
  while (!wifi_connect())
  {
    //
  }

  if (wifi_isConnected())
  {
    // Корректируем дату и время
    setClock();
    // Подключаемся к Iot Core
    while (!mqtt_connect())
    {
      ;
    }
  }

  if (mqtt_isConnected())
    ;

  // Инициализируем барометр
  barometer.begin();
  delay(100);

  delay(5000);

  if (DEBUG)
  {
    DEBUG_SERIAL.println("Start ...");
    DEBUG_SERIAL.println();
  }
}

void loop()
{
  if (wifi_isConnected())
  {
    if (!mqtt_client.loop())
    {
      // Подключаемся к Iot Core
      while (!mqtt_connect())
      {
        ;
      }
    }
    else
    {
      if (first_flag == true)
      {
        first_flag = false;
        readWeatherData();
      }
      else if (first_flag == false)
      {
        static unsigned long lastTempRead = 0;
        if (((millis() - lastTempRead) >= 7 * 60000))
        {
          lastTempRead = millis();
          if (lvlRelayFlag == 0x01) // если метеостанция включена
          {
            readWeatherData();
          }
        };
      }
    }
  }
  else
  {
    while (!wifi_connect())
    {
      ;
    }
  }

  if (wifi_isConnected())
    ;
  if (mqtt_isConnected())
    ;

  delay(1000);
}
