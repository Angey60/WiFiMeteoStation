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