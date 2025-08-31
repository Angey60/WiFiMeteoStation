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


void update_started()
{
  DEBUG_SERIAL.println("CALLBACK:  HTTP update process started");
}

void update_finished()
{
  DEBUG_SERIAL.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total)
{
  DEBUG_SERIAL.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err)
{
  DEBUG_SERIAL.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

// CALLBACK:  HTTP update fatal error code -101
// HTTP_UPDATE_FAILD Error (-101): Server Did Not Report Size
void otaStart(const char *linkOTA)
{
  DEBUG_SERIAL.print("OTA :: Receiving OTA: ");
  DEBUG_SERIAL.print(linkOTA);
  DEBUG_SERIAL.println(" ...");

  // Корректируем дату и время
  setClock();

  // Запускаем обновление прошивки
  WiFiClientSecure otaWiFi;
  otaWiFi.setTrustAnchors(&cert);

  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  // Add optional callback notifiers
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);

  // Запускаем обновление
  t_httpUpdate_return ret = ESPhttpUpdate.update(otaWiFi, linkOTA);
  // Анализируем результат
  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    DEBUG_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    DEBUG_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    DEBUG_SERIAL.println("HTTP_UPDATE_OK");
    break;
  };
  DEBUG_SERIAL.println();
}

void connect()
{
  delay(500);

  DEBUG_SERIAL.print(F("Conecting to wifi ..."));
  while (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_SERIAL.print(F("."));
    delay(500);
  }
  DEBUG_SERIAL.println(F(" Connected"));

  net.setInsecure();

  DEBUG_SERIAL.print(F("Connecting to Yandex IoT Core as"));
  DEBUG_SERIAL.print(yandexIoTCoreBrokerId);
  DEBUG_SERIAL.print(F(" ..."));
  while (!mqtt_client.connect("angey60_Esp8266Client_broker", yandexIoTCoreBrokerId, mqttpassword))
  {
    DEBUG_SERIAL.print(F("."));
    delay(500);
  }
  DEBUG_SERIAL.println(F(" Connected"));

  DEBUG_SERIAL.print(F("Subscribe to: "));
  DEBUG_SERIAL.print(commands);
  DEBUG_SERIAL.print(" - ");
  DEBUG_SERIAL.print(events);
  DEBUG_SERIAL.println("\r\n");

  mqtt_client.subscribe(commands);
  mqtt_client.subscribe(events);
  mqtt_client.subscribe(commands_01.c_str());
}

// Подключение к WiFi точке доступа
bool wifiConnected()
{
  // Если подключение активно, то просто выходим и возвращаем true
  if (WiFi.status() != WL_CONNECTED)
  {
    // ... иначе пробуем подключиться к сети
    if (DEBUG)
    {
      DEBUG_SERIAL.print(F("Connecting to WiFi AP "));
      DEBUG_SERIAL.print(SSID);
      DEBUG_SERIAL.print(" ");
    }
    // Настраиваем WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASS);

    digitalWrite(gpioRelay, 0x01);

    // И ждем подключения 60 циклов по 0,5 сек - это 30 секунд
    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
      i++;
      if (i > 60)
      {
        // Если в течение этого времени не удалось подключиться - выходим с false
        // Бескорнечно ждать подключения опасно - если подключение было разорвано во время работы
        // нужно всё равно "обслуживать" реле и датчики, иначе может случиться беда
        if (DEBUG)
        {
          DEBUG_SERIAL.println("");
          DEBUG_SERIAL.println(F("Connection failed!"));
        }
        return false;
      };
      if (DEBUG)
      {
        DEBUG_SERIAL.print(".");
      }
      delay(500);
    };

    if (WiFi.status() == WL_CONNECTED)
    {
      // Подключение успешно установлено
      if (DEBUG)
      {
        DEBUG_SERIAL.println(F(" ок"));
        DEBUG_SERIAL.print(F("WiFi connected, obtained IP address: "));
        DEBUG_SERIAL.println(WiFi.localIP());

        digitalWrite(gpioRelay, 0x00);
      }
    }
  }

  return (WiFi.status() == WL_CONNECTED);
}

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
