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

    // DEBUG_SERIAL.println("command: "+command);

    if (command == "1")
    {
        expander.digitalWrite(expander_gpioRelay, lvlRelayOn);
        return;
    }

    if (command == "0")
    {
        expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
        return;
    }

    if (command == "7") // обновление
    {
        // CALLBACK:  HTTP update fatal error code -1
        // HTTP_UPDATE_FAILD Error (-1): HTTP error: connection failed
        // DEBUG_SERIAL.println("Start OTA ubdate ...");
        otaStart(firmware_url.c_str());
        return;
    }
    
}