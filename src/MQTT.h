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
        expander.digitalWrite(expander_gpioRelay, lvlRelayOn);
        return;
    }

    if (command == "0")
    {
        // digitalWrite(BUILTIN_LED, HIGH);
        // setColor(m_current, RGB::Black());
        expander.digitalWrite(expander_gpioRelay, lvlRelayOff);
        return;
    }
}