/**************************************************************************
 * NET функции
 *************************************************************************/
// Подключение к WiFi точке доступа
bool wifi_connect()
{
    // Если подключение активно, то просто выходим и возвращаем true
    if (!WiFi.isConnected())
    {
        // ... иначе пробуем подключиться к сети
        if (DEBUG)
        {
            DEBUG_SERIAL.print(F("Connecting to WiFi AP "));
            DEBUG_SERIAL.print(SSID);
            DEBUG_SERIAL.print(F(" "));
        }
        // Настраиваем WiFi
        WiFi.mode(WIFI_STA);
        WiFi.begin(SSID, PASS);
        //
        digitalWrite(gpioRelay, lvlRelayOn);
        //
        // И ждем подключения 60 циклов по 0,5 сек - это 30 секунд
        int i = 0;
        while (!WiFi.isConnected())
        {
            i++;
            if (i > 60)
            {
                // Если в течение этого времени не удалось подключиться - выходим с false
                // Бескорнечно ждать подключения опасно - если подключение было разорвано во время работы
                // нужно всё равно "обслуживать" реле и датчики, иначе может случиться беда
                if (DEBUG)
                {
                    DEBUG_SERIAL.println();
                    DEBUG_SERIAL.println(F("Connection failed!"));
                }
                return false;
            };
            if (DEBUG)
            {
                DEBUG_SERIAL.print(F("."));
            }
            delay(500);
        };

        if (WiFi.isConnected())
        {
            // Подключение успешно установлено
            if (DEBUG)
            {
                DEBUG_SERIAL.println(F(" ок"));
                DEBUG_SERIAL.print(F("WiFi connected, obtained IP address: "));
                DEBUG_SERIAL.println(WiFi.localIP());
            }
            digitalWrite(gpioRelay, lvlRelayOff);
        }
    }

    return (WiFi.isConnected());
}

void wifi_disconnect()
{
    if (WiFi.isConnected())
    {
        WiFi.disconnect();
    };
}

bool wifi_isConnected()
{
    bool flag = WiFi.isConnected();
    // индикатор WiFi
    digitalWrite(gpioRelay, flag);
    delay(50);
    return flag;
}