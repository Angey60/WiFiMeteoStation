#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <time.h>
//
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SoftwareSerial.h>
#define BLE_RX A5
#define BLE_TX A6
SoftwareSerial ble_serial(BLE_TX, BLE_RX);
#define BLE_SERIAL ble_serial

#include <Adafruit_I2CDevice.h>
// библиотека для работы с модулем Slot Expander (I²C IO)
#include <GpioExpander.h>
// создаём объект expander класса GpioExpander по адресу 43
GpioExpander expander(43);
// создаём объект expander класса GpioExpander по адресу 45
GpioExpander expander_1(42);
#include <Adafruit_BusIO_Register.h>
#include <TroykaIMU.h>
// библиотека для работы с OLED-дисплеем
#include <TroykaOLED.h>
// создаём объект для работы с дисплеем
// и передаём I²C адрес дисплея
TroykaOLED myOLED(0x3C);
// #include <QuadDisplay2.h>
//  создаём объект класса QuadDisplay и передаём номера пинов CS, DI и ⎍
// QuadDisplay qd(A2);
#include <constants.h>
// WiFi клиент
#include <MyWiFi.h>
MyWiFi wifi_client;
// #include <MyBLE.h>
// MyBLE ble_client;
//  MQTT клиент
#include <MyMQTT.h>
MyMQTT mqtt_client;
#include <MyOTA.h>
MyOTA ota_client;
#include <MyClock.h>
MyClock clock_client;
#include <MyLittleFS.h>
MyLittleFS littlefs;
//
#include <service_functions.h>
// Функция обратного вызова при поступлении входящего сообщения от брокера
void callback(char *topic, byte *payload, unsigned int length);
//
void ble_init();

void setup()
{
  delay(2500);

  // put your setup code here, to run once:
  Wire.begin();
  // Инициализируем бортовые часы
  // initClock();
  clock_client.begin();
  // Инициализируем объект expander.
  expander.begin();
  CustomDelay(500);
  // Инициализируем объект expander_1.
  expander_1.begin();
  CustomDelay(500);
  // Инициализируем индикаторы
  // красная лампочка On/Off WiFi
  pinMode(gpioWiFi, OUTPUT);
  expander.digitalWrite(gpioWiFi, gpioSignalOff);
  // зеленая лампочка On/Off метеостанции
  expander.pinMode(gpioOnOff, OUTPUT);
  expander.digitalWrite(gpioOnOff, gpioSignalOff);
  // синяя лампочка On/Off MQTT
  expander.pinMode(gpioMQTT, OUTPUT);
  expander.digitalWrite(gpioMQTT, gpioSignalOff);
  // желтая лампочка On/Off BLE
  expander.pinMode(gpioBLE, OUTPUT);
  expander.digitalWrite(gpioBLE, gpioSignalOff);
  //
  DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
  while (!DEBUG_SERIAL)
  {
    ;
  }
  CustomDelay(500);

  BLE_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
  while (!BLE_SERIAL)
  {
    ;
  }
  CustomDelay(500);

  if (BLE_SERIAL)
  {
    if (DEBUG)
    {
      DEBUG_SERIAL.println(F("BLE functions ..."));
    }
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
  delay(500);
  expander.digitalWrite(gpioOnOff, gpioSignalOff);
  delay(500);
  // Индикатор включения/отклячения MQTT
  expander.digitalWrite(gpioMQTT, gpioSignalOn);
  delay(500);
  expander.digitalWrite(gpioMQTT, gpioSignalOff);
  delay(500);
  // Индикатор включения/отклячения BLE
  expander.digitalWrite(gpioBLE, gpioSignalOn);
  delay(500);
  expander.digitalWrite(gpioBLE, gpioSignalOff);
  delay(500);

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

  // инициализируем дисплей
  myOLED.begin();
  // выбираем шрифт 6×8
  myOLED.setFont(font6x8);
  // печатаем строку
  myOLED.print("Hello world!", 25, 0);
  // выбираем шрифт 12×10
  myOLED.setFont(font12x10);
  // печатаем строку
  myOLED.print("Amperka", 25, 20);
  // инвертируем последующий текст
  myOLED.invertText("true");
  myOLED.print("OLED", 42, 45);
  // Инициализируем метеостанцию
  SHT3x.begin();
  // Инициализируем барометр
  barometer.begin();
  // Делаем задержку перед обращением к серверу
  delay(2500);

  if (DEBUG)
  {
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println("Start ...");
    DEBUG_SERIAL.println();

    DEBUG_SERIAL.println();
    DEBUG_SERIAL.print("ESP Board MAC Address:  ");
    DEBUG_SERIAL.println(mac_address());
    DEBUG_SERIAL.println();
  }
  //
  //
  //
  if (littlefs.begin())
  {
    if (DEBUG)
    {
      if (littlefs.checkFile("/text.txt"))
      {
        DEBUG_SERIAL.println(littlefs.readFile("/text.txt"));
        DEBUG_SERIAL.println();
        littlefs.appendFile("/text.txt", "Тяжела и неказиста жизнь кота у программиста");
        DEBUG_SERIAL.println();
        DEBUG_SERIAL.println(littlefs.readFile("/text.txt"));
        DEBUG_SERIAL.println();
      }
      
    }
  }
  else
  {
    if (DEBUG)
    {
      Serial.println(F("LittleFS mount failed"));
    }
  }
}

void loop()
{
  /*if (wifi_client.isConnected())
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
        if (((millis() - weather_data_last_temp_read) >= 3 * 60000))
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
  // Контроль включение BLE
  if (BLE_SERIAL)
  {
    expander.digitalWrite(gpioBLE, gpioSignalOn);
  }
  else if (!BLE_SERIAL)
  {
    expander.digitalWrite(gpioBLE, gpioSignalOff);
  }
  CustomDelay(100);
  // Контроль включения метеостанции
  if (meteo_station_gpio_status())
    ;
    */
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

  if (command == "9")
  {

    // ble_init();
  }
}

void ble_init()
{
  if (DEBUG)
  {
    // /yandex-iot-core/device-iot-wifi_slot/commands
    if (DEBUG)
    {
      DEBUG_SERIAL.println(F("ble_serial init ..."));
    }
    if (!BLE_SERIAL)
    {
      BLE_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
      while (!BLE_SERIAL)
      {
        delay(10);
        DEBUG_SERIAL.print(".");
      }
      DEBUG_SERIAL.println();
    }
    if (BLE_SERIAL)
    {
      BLE_SERIAL.println(F("BLE functions ..."));

      if (DEBUG)
      {
        DEBUG_SERIAL.println(F("BLE functions ..."));
      }
    }
  }
}
