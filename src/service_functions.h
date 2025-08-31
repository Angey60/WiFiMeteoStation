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

// Set time via NTP, as required for x.509 validation
void setClock()
{
  // Для работы TLS-соединения нужны корректные дата и время, получаем их с NTP серверов
  configTime("MSK-3", "pool.ntp.org", "time.nist.gov");

  if (DEBUG)
  {
    Serial.print(F("Waiting for NTP time sync: "));
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
      }
      if (DEBUG)
      {
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
  }
  if (DEBUG)
  {
    DEBUG_SERIAL.print(asctime(&timeinfo));
  }
}
