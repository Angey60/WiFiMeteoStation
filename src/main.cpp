#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <time.h>
#include <RTClib.h>
// создаём объект для работы с часами реального времени
RTC_DS1307 rtc;

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>
#include <LittleFS.h>

#include <ArduinoJson.h>
JsonDocument doc;
char json_buffer[200] = {0};
char json_buffer_err[50] = {0};
#define BUFFER_LENGTH 128
char buffer[BUFFER_LENGTH];

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
// библиотека для работы с метео сенсором
#include <TroykaMeteoSensor.h>
// библиотека для работы с часами реального времени
// создаём объект для работы с датчиком
TroykaMeteoSensor SHT3x;
// создаём объект для работы с часами реального времени
// RTC_DS1307 rtc;
// Создаём объект для работы с акселерометром
Barometer barometer;

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
  // Инициализируем бортовые часы
  initClock();
  //  Инициализируем объект display
  qd.begin();
  // Инициализируем объект expander.
  expander.begin();
  // Инициализируем индикаторы
  // красная лампочка On/Off WiFi
  pinMode(gpioRelay, OUTPUT);
  // зеленая лампочка On/Off метеостанции
  expander.pinMode(expander_gpioRelay, OUTPUT);
  expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
  // синяя лампочка On/Off MQTT !!!
  expander.pinMode(expander_gpioRelay1, OUTPUT);
  expander.digitalWrite(expander_gpioRelay1, lvlRelayOff);
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
  expander.digitalWrite(expander_gpioRelay, lvlRelayOn);
  delay(500);
  expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
  delay(500);

  // Индикатор включения/отклячения метеостанции
  expander.digitalWrite(expander_gpioRelay1, lvlRelayOn);
  delay(500);
  expander.digitalWrite(expander_gpioRelay1, lvlRelayOff);
  delay(500);

  /*
  if (!LittleFS.begin())
  {
    DEBUG_SERIAL.println("An Error has occurred while mounting LittleFS");
    // return;
  }

  File file = LittleFS.open("/text.txt", "r");
  if (!file)
  {
    DEBUG_SERIAL.println("Failed to open file for reading");
    // return;
  }

  DEBUG_SERIAL.print("File Content: ");
  while (file.available())
  {
    DEBUG_SERIAL.write(file.read());
  }
  file.close();
  */

  DEBUG_SERIAL.println(F("\r\n"));

  MQTClient();

  // Подключаемся к WiFi
  if (wifi_connected())
  {
    // Корректируем дату и время
    setClock();
    // Подключаемся к Iot Core
    while (!mqtt_connect())
    {
      ;
    }
    if (mqtt_isConnect())
      ;
  }

  // Инициализируем барометр
  barometer.begin();
  delay(100);
  //
  String s = readWeatherData(false);
  DEBUG_SERIAL.println(s);
  delay(100);

  while (!wifi_connected())
  {
    ;
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  mqtt_client.loop();

  if (!mqtt_client.connected())
  {
    // Корректируем дату и время
    setClock();
    // Подключаемся к Iot Core
    while (!mqtt_connect())
    {
      ;
    }
  }

  // Проверяем состояние WiFi и MQTT
  if (mqtt_isConnect())
    ;
  if (wifi_isConnect())
    ;

  static unsigned long lastTempRead = 0;
  if (((millis() - lastTempRead) >= 30 * 60000) || (not first_flag))
  {
    first_flag = true;
    lastTempRead = millis();
    if (lvlRelayOn == 0x01) // ??????
    {
      readWeatherData(true);
    }
  };
}
