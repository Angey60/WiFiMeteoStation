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