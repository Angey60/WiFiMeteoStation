/**************************************************************************
 * Сервисные функции
 *************************************************************************/
#include <ArduinoJson.h>
JsonDocument doc;
char json_buffer[200] = {0};
char json_buffer_err[50] = {0};
#define BUFFER_LENGTH 128
char buffer[BUFFER_LENGTH];

// библиотека для работы с метео сенсором
#include <TroykaMeteoSensor.h>
// библиотека для работы с часами реального времени
// создаём объект для работы с датчиком
TroykaMeteoSensor SHT3x;
// Создаём объект для работы с барометром
Barometer barometer;

String ConvertCharToString(const char ch[])
{
  String result = "";
  int len = strlen_P(ch);
  for (int i = 0; i < len; i++)
  {
    char c = pgm_read_byte_near(ch + i);
    result += c;
    delay(10);
  }
  return result;
}

String mac_address(void)
{
  uint8_t mac[6];
  char macStr[18] = {0};
  wifi_get_macaddr(STATION_IF, mac);

  sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}

// Чтение температуры и влажности
char *readWeatherData()
{
  // считываем данные с датчика
  float float_temp = 0;
  String str_temp("");
  float float_humd = 0;
  String str_humd("");
  String str_mill("");

  int stateSensor = SHT3x.read();
  delay(250);
  // проверяем состояние данных
  switch (stateSensor)
  {
  case SHT_OK:
    // Публикуем данные на сервере
    float_temp = SHT3x.getTemperatureC();
    delay(100);
    str_temp = String(float_temp);
    //  выводим на терминал
    qd.displayTemperatureC(int(SHT3x.getTemperatureC()));
    float_humd = SHT3x.getHumidity();
    delay(50);
    str_humd = String(float_humd);
    if (DEBUG)
    {
      DEBUG_SERIAL.print(F("Temperature = "));
      DEBUG_SERIAL.print(SHT3x.getTemperatureC());
      DEBUG_SERIAL.println(F(" C \t"));
      DEBUG_SERIAL.print(F("Humidity = "));
      DEBUG_SERIAL.print(SHT3x.getHumidity());
      DEBUG_SERIAL.print(" %");
      DEBUG_SERIAL.println(F("\t"));
    }
    break;

  case SHT_ERROR_DATA:
    if (DEBUG)
    {
      DEBUG_SERIAL.println(F("Data error or sensor not connected"));
    }
    break;

  // ошибка контрольной суммы
  case SHT_ERROR_CHECKSUM:
    if (DEBUG)
    {
      DEBUG_SERIAL.println(F("Checksum error"));
    }
    break;
  };

  //  Создаём переменную для значения атмосферного давления в мм рт.ст.
  float pressureMillimetersHg = barometer.readPressureMillimetersHg();
  // Создаём переменную для значения высоты над уровнем море
  // float altitude = barometer.readAltitude();
  delay(100);

  str_mill = String(pressureMillimetersHg);
  if (DEBUG)
  {
    DEBUG_SERIAL.print(F("Pressure = "));
    DEBUG_SERIAL.print(pressureMillimetersHg);
    DEBUG_SERIAL.print(F(" mmHg\t"));
    DEBUG_SERIAL.println(F("\r\n"));
  }
  String buffer_st = "";

  buffer_st = String("device-iot-wifi_slot");
  buffer_st.trim();
  doc["device"] = buffer_st;

  buffer_st = String(mac_address());
  buffer_st.trim();
  doc["mac"] = buffer_st;

  dtostrf(float_temp, 6, 2, buffer);
  buffer_st = String(buffer);
  buffer_st.trim();
  doc["temperature"] = buffer_st;

  dtostrf(float_humd, 6, 2, buffer);
  buffer_st = String(buffer);
  buffer_st.trim();
  doc["humidity"] = buffer_st;

  dtostrf(pressureMillimetersHg, 6, 2, buffer);
  buffer_st = String(buffer);
  buffer_st.trim();
  doc["pressure"] = buffer_st;

  serializeJson(doc, json_buffer);
  // чистим буфер
  deserializeJson(doc, json_buffer_err);

  if (DEBUG)
  {
    DEBUG_SERIAL.println(json_buffer);
    DEBUG_SERIAL.println();
  }

  return json_buffer;
}

bool meteo_station_gpio_status()
{
  bool flag = mqtt_client.status;
  // индикатор включения метеостанции
  if (flag)
  {
    expander.digitalWrite(gpioOnOff, 0x1);
    delay(1000);
    expander.digitalWrite(gpioOnOff, 0x0);
  }
  else
  {
    expander.digitalWrite(gpioOnOff, 0x0);
  }
  delay(50);
  return flag;
}
