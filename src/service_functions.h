/**************************************************************************
 * Сервисные функции
 *************************************************************************/

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

void initClock()
{
  // настраиваем часы реального времени
  if (!rtc.begin())
  {
    while (1)
      ;
  }
  if (!rtc.isrunning())
  {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// Set time via NTP, as required for x.509 validation
void setClock()
{
  // Для работы TLS-соединения нужны корректные дата и время, получаем их с NTP серверов
  configTime("MSK-3", "pool.ntp.org", "time.nist.gov");

  if (DEBUG)
  {
    DEBUG_SERIAL.print(F("Waiting for NTP time sync: "));
  }
  int i = 0;
  time_t now = time(nullptr);
  while (now < 1000000000)
  {
    now = time(nullptr);
    i++;
    if (i > 60)
    {
      // Если в течение этого времени не удалось подключиться - выходим с false
      // Бескорнечно ждать подключения опасно - если подключение было разорвано во время работы
      // нужно всё равно "обслуживать" реле и датчики, иначе может случиться беда
      if (DEBUG)
      {
        DEBUG_SERIAL.println();
        DEBUG_SERIAL.println(F("Time sync failed!"));
      }
      // return false;
    };
    if (DEBUG)
    {
      DEBUG_SERIAL.print(F("."));
    }
    delay(500);
  }

  // Время успешно синхронизировано, выводим его в монитор порта
  // Только для целей отладки, этот блок можно исключить
  if (DEBUG)
  {
    DEBUG_SERIAL.println(F(" ok"));
  }

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  if (DEBUG)
  {
    DEBUG_SERIAL.print(F("Current time: "));
    DEBUG_SERIAL.print(asctime(&timeinfo));
    DEBUG_SERIAL.println();
  }
  // корректируем время бортовых часов
  rtc.adjust(DateTime(
      2000 + (timeinfo.tm_year - 100),
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec));

  if (DEBUG)
  {
    DateTime now1 = rtc.now();
    DEBUG_SERIAL.print(F("Date & Time: "));
    DEBUG_SERIAL.print(now1.year(), DEC);
    DEBUG_SERIAL.print(F("/"));
    DEBUG_SERIAL.print(now1.month(), DEC);
    DEBUG_SERIAL.print(F("/"));
    DEBUG_SERIAL.print(now1.day(), DEC);
    DEBUG_SERIAL.print(F(" ("));
    DEBUG_SERIAL.print(now1.dayOfTheWeek());
    DEBUG_SERIAL.print(F(") "));
    DEBUG_SERIAL.print(now1.hour(), DEC);
    DEBUG_SERIAL.print(F(":"));
    DEBUG_SERIAL.print(now1.minute(), DEC);
    DEBUG_SERIAL.print(F(":"));
    DEBUG_SERIAL.println(now1.second(), DEC);
  }
}

// Чтение температуры и влажности
String readWeatherData(bool flag = true)
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

  // delay(100);
  //  Создаём переменную для значения атмосферного давления в Паскалях
  // float pressurePascals = barometer.readPressurePascals();
  //  Создаём переменную для значения атмосферного давления в мм рт.ст.
  float pressureMillimetersHg = barometer.readPressureMillimetersHg();
  // Создаём переменную для значения высоты над уровнем море
  // float altitude = barometer.readAltitude();
  delay(100);

  str_mill = String(pressureMillimetersHg);
  if (DEBUG)
  {
    DEBUG_SERIAL.print(pressureMillimetersHg);
    // DEBUG_SERIAL.print(str_mill.c_str());
    DEBUG_SERIAL.print(F(" mmHg\t"));
    DEBUG_SERIAL.println(F("\r\n"));
  }
  String buffer_st = "";

  // dtostrf(float_temp, 6, 2, buffer);
  buffer_st = String("device-iot-wifi_slot");
  buffer_st.trim();
  doc["device"] = buffer_st;

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

  if ((flag == true))
  {
    mqtt_client.publish(mqttTopicParamerters, json_buffer, mqttSensorsRetained);
  }

  return json_buffer;
}
