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



// put function declarations here:
int myFunction(int, int);

// Функция обратного вызова при поступлении входящего сообщения от брокера
void callback(char *topic, byte *payload, unsigned int length)
{
  // Для более корректного сравнения строк приводим их к нижнему регистру и обрезаем пробелы с краев
  String _payload;
  for (unsigned int i = 0; i < length; i++)
  {
    _payload += String((char)payload[i]);
  };

  _payload.toLowerCase();
  _payload.trim();

  // Вывод поступившего сообщения в лог, больше никакого смысла этот блок кода не несет, можно исключить
  DEBUG_SERIAL.print(F("Message arrived ["));
  DEBUG_SERIAL.print(topic);
  DEBUG_SERIAL.print(F("]: "));
  DEBUG_SERIAL.print(_payload.c_str());
  DEBUG_SERIAL.println("");

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
    // digitalWrite(BUILTIN_LED, LOW);
    // setColor(RGB::Black(), m_current);
    // expander.digitalWrite(expander_gpioRelay, lvlRelayOn);
    return;
  }

  if (command == "0")
  {
    // digitalWrite(BUILTIN_LED, HIGH);
    // setColor(m_current, RGB::Black());
    // expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
    return;
  }
}

void setup()
{
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
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
  // expander.pinMode(expander_gpioRelay, OUTPUT);
  // expander.digitalWrite(expander_gpioRelay, lvlRelayOn);
  // delay(1000);
  // expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
  delay(1000);

  // настраиваем часы реального времени
  // initClock();

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
