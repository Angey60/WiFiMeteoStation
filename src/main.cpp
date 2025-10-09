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
//include <MQTT.h>
#include <LITTLEFS1.h>
#include <MyMQTT.h>

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
  //MQTClient();

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

  if (wifi_gpio_status())
    ;

  if (mqtt_gpio_status())
    ;

  if (meteo_station_gpio_status())
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
        readWeatherData();
        first_flag = false;
      }
      else if (first_flag == false)
      {
        static unsigned long weather_data_last_temp_read = 0;
        if (((millis() - weather_data_last_temp_read) >= 7 * 60000))
        {
          weather_data_last_temp_read = millis();
          if (lvlRelayFlag == 0x01) // если метеостанция включена
          {
            readWeatherData();
          }
        };
        
        /*static unsigned long meteo_station_last_temp_read = 0;
        if (((millis() - meteo_station_last_temp_read) >= 1 * 1000))
        {
          meteo_station_last_temp_read = millis();
          if (mqtt_gpio_triggr_on == 0x0) // если индикатор выключен
          {
            mqtt_gpio_triggr_on = 0x1; // включаем на 500 мсек
            *static unsigned long mqtt_gpio_triggr_last_temp_read = 0;
            if (((millis() - meteo_station_last_temp_read) >= 500))
            {
              meteo_station_last_temp_read = millis();
            }*
          }
        }*/
      }
    }
    delay(500);
  }
  else
  {
    while (!wifi_connect())
    {
      ;
    }
  }
  // Контроль подключения к WiFi
  if (wifi_gpio_status())
    ;
  // Контроль подключения к MQTT-серверу
  if (mqtt_gpio_status())
    ;
    // Контроль включения метеостанции
  if (meteo_station_gpio_status())
    ;
}
