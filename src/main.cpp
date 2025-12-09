#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <time.h>
//
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//
#include <SoftwareSerial.h>
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
#include <constants.h>
#include <LITTLEFS1.h>
// WiFi клиент
#include <MyWiFi.h>
MyWiFi wifi_client;
// MQTT клиент
#include <MyMQTT.h>
MyMQTT mqtt_client;
#include <MyOTA.h>
MyOTA ota_client;
#include <MyClock.h>
MyClock clock_client;
//
#include <service_functions.h>
#define BUTTON_PIN 3

// Функция обратного вызова при поступлении входящего сообщения от брокера
void callback(char *topic, byte *payload, unsigned int length);

void setup()
{
  // put your setup code here, to run once:
  Wire.begin();
  // Инициализируем бортовые часы
  // initClock();
  clock_client.begin();
  //  Инициализируем объект display
  qd.begin();
  // Инициализируем объект expander.
  expander.begin();
  // Инициализируем индикаторы
  // красная лампочка On/Off WiFi
  pinMode(gpioWiFi, OUTPUT);
  // зеленая лампочка On/Off метеостанции
  expander.pinMode(gpioOnOff, OUTPUT);
  expander.digitalWrite(gpioOnOff, gpioSignalOff);
  // синяя лампочка On/Off MQTT !!!
  expander.pinMode(gpioMQTT, OUTPUT);
  expander.digitalWrite(gpioMQTT, gpioSignalOff);
  // настраиваем пин в режим входа
  expander.pinMode(BUTTON_PIN, INPUT);
  // Инициализируем метеостанцию
  // SHT3x.begin();

  DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
  while (!DEBUG_SERIAL)
  {
    ;
  }
  CustomDelay(500);

  if (DEBUG)
  {
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println(F("Device started ..."));
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.flush();
    CustomDelay(500);
  }

  if (DEBUG)
  {
    DEBUG_SERIAL.println(F("Meteo Sensor init OK"));
    DEBUG_SERIAL.println(F("Demo project FOR ESP8266"));
    DEBUG_SERIAL.flush();
  }

  // Индикатор включения/отклячения метеостанции
  expander.digitalWrite(gpioOnOff, gpioSignalOn);
  CustomDelay(500);
  expander.digitalWrite(gpioOnOff, gpioSignalOff);
  CustomDelay(500);
  // Индикатор включения/отклячения метеостанции
  expander.digitalWrite(gpioMQTT, gpioSignalOn);
  CustomDelay(500);
  expander.digitalWrite(gpioMQTT, gpioSignalOff);
  CustomDelay(500);

  wifi_client.disconnect();
  if (DEBUG)
  {
    DEBUG_SERIAL.println(F("\r\n"));
  }
  // Подключаемся к WiFi
  while (!wifi_client.connect())
  {
    //
  }

  if (wifi_client.isConnected())
  {
    // Корректируем дату и время
    clock_client.setClock();
    // Подключаемся к Iot Core
    // Активизируем WQTT клиент
    mqtt_client.begin();
    while (!mqtt_client.connect())
    {
      ;
    }
    mqtt_client.setCallback(callback);
  }
  // Контроль подключения к WiFi
  digitalWrite(gpioWiFi, wifi_client.gpio_status());
  // Контроль подключения к MQTT-серверу
  expander.digitalWrite(gpioMQTT, mqtt_client.gpio_status());
  // Контроль включения метеостанции
  if (meteo_station_gpio_status())
    ;

  // Инициализируем метеостанцию
  SHT3x.begin();
  // Инициализируем барометр
  barometer.begin();

  // Делаем задержку перед обращением к серверу
  delay(5000);

  if (DEBUG)
  {
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println("Start ...");
    DEBUG_SERIAL.println();

    DEBUG_SERIAL.println();
    DEBUG_SERIAL.print("ESP Board MAC Address:  ");
    DEBUG_SERIAL.println(mac_address());
  }
}

void loop()
{
  if (wifi_client.isConnected())
  {
    if (!mqtt_client.loop())
    {
      // Подключаемся к Iot Core
      while (!mqtt_client.connect())
      {
        ;
      }
      mqtt_client.setCallback(callback);
    }
    else
    {
      if (first_flag == true)
      {
        if (mqtt_client.isConnected())
        {
          mqtt_client.publish(readWeatherData());
        }
        first_flag = false;
      }
      else if (first_flag == false)
      {
        static unsigned long weather_data_last_temp_read = 0;
        if (((millis() - weather_data_last_temp_read) >= 60 * 60000))
        {
          weather_data_last_temp_read = millis();
          if (mqtt_client.status == 0x01) // если метеостанция включена
          {
            if (mqtt_client.isConnected())
            {
              mqtt_client.publish(readWeatherData());
            }
          }
        };
      }
    }
    delay(500);
  }
  else
  {
    while (!wifi_client.connect())
    {
      ;
    }
  }
  // Контроль подключения к WiFi
  digitalWrite(gpioWiFi, wifi_client.gpio_status());
  // Контроль подключения к MQTT-серверу
  expander.digitalWrite(gpioMQTT, mqtt_client.gpio_status());
  // считываем состояние пина
  /*int buttonState = expander.digitalRead(BUTTON_PIN);
  if (DEBUG)
  {
    // выводим в Serial-порт
    DEBUG_SERIAL.println(buttonState);
  }*/
  CustomDelay(100);
  // Контроль включения метеостанции
  if (meteo_station_gpio_status())
    ;
}

// Функция обратного вызова при поступлении входящего сообщения от брокера
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("runing callback ...");
  // Для более корректного сравнения строк приводим их к нижнему регистру и обрезаем пробелы с краев
  String _payload;
  for (unsigned int i = 0; i < length; i++)
  {
    _payload += String((char)payload[i]);
  };

  _payload.toLowerCase();
  _payload.trim();

  // Вывод поступившего сообщения в лог, больше никакого смысла этот блок кода не несет, можно исключить
  if (DEBUG)
  {
    DEBUG_SERIAL.print(F("Message arrived ["));
    DEBUG_SERIAL.print(topic);
    DEBUG_SERIAL.print(F("]: "));
    DEBUG_SERIAL.print(_payload.c_str());
    DEBUG_SERIAL.println("");
  }

  int pos = 0;
  String command = "";
  for (unsigned int i = 0; i < length; i++)
  {
    if ((char)payload[i] == '=')
      break;
    command += (char)payload[i];
    pos++;
  }

  if (command == "1")
  {
    expander.digitalWrite(gpioMQTT, gpioSignalOn);
    mqtt_client.status = 0x1;
    return;
  }

  if (command == "0")
  {
    expander.digitalWrite(gpioMQTT, gpioSignalOff);
    mqtt_client.status = 0x0;
    return;
  }

  if (command == "7") // обновление
  {
    // Корректируем дату и время
    clock_client.setClock();
    // Закрываем соединение с MQTT-брокером
    mqtt_client.disConnect();
    CustomDelay(1000);
    ota_client.begin();
    return;
  }
}

/*
#define BLE_RX 10
#define BLE_TX 11
SoftwareSerial ble_serial(BLE_RX, BLE_TX);
#define BLE_SERIAL ble_serial
*/