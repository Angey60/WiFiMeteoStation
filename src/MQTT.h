bool mqtt_connect()
{
    delay(500);

    if (DEBUG)
    {
        DEBUG_SERIAL.print(F("Conecting to wifi ..."));
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        if (DEBUG)
        {
            DEBUG_SERIAL.print(F("."));
        }
        delay(500);
    }

    if (DEBUG)
    {
        DEBUG_SERIAL.println(F(" Connected"));
    }

    if (DEBUG)
    {
        DEBUG_SERIAL.print(F("Connecting to Yandex IoT Core as"));
        DEBUG_SERIAL.print(yandexIoTCoreBrokerId);
        DEBUG_SERIAL.print(F(" ..."));
    }

    while (!mqtt_client.connect("angey60_Esp8266Client_broker", yandexIoTCoreBrokerId, mqttpassword))
    {
        if (DEBUG)
        {
            DEBUG_SERIAL.print(F("."));
        }
        delay(500);
    }

    if (DEBUG)
    {
        DEBUG_SERIAL.println(F(" Connected"));
        DEBUG_SERIAL.print(F("Subscribe to: "));
        DEBUG_SERIAL.print(commands);
        DEBUG_SERIAL.print(F(" - "));
        DEBUG_SERIAL.print(events);
        DEBUG_SERIAL.println(F("\r\n"));
    }

    mqtt_client.subscribe(commands);
    mqtt_client.subscribe(events);
    mqtt_client.subscribe(commands_01.c_str());

    return mqtt_client.connected();
}

bool mqtt_isConnect()
{
    bool flag = mqtt_client.connected();
    // индикатор MQTT
    expander.digitalWrite(expander_gpioRelay1, flag);
    return flag;
}

void mqtt_disconnect()
{
    if (mqtt_client.connected())
    {
        mqtt_client.disconnect();
    };
}

// Функция обратного вызова при поступлении входящего сообщения от брокера
void mqtt_callback(char *topic, byte *payload, unsigned int length)
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
        // Корректируем дату и время
        setClock();
        // Закрываем соединение с MQTT-брокером
        mqtt_disconnect();
        delay(1000);
        otaStart(firmware_url.c_str());
        return;
    }
}

void MQTClient()
{
    // Привязываем корневой сертификат к клиенту Iot Core
    mqttServ.setTrustAnchors(&mqttCert);
    // Настраиваем клиент Iot Core
    mqtt_client.setServer(mqttserver, mqttport);
    mqtt_client.setCallback(mqtt_callback);
    mqtt_client.setBufferSize(1024);
    mqtt_client.setKeepAlive(15);
}