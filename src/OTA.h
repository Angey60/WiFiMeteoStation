/**************************************************************************
 * OTA функции
 *************************************************************************/
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266httpUpdate.h>

String firmware_url = "https://angey60meteo.ru/firmware.bin";

void update_started()
{
    if (DEBUG)
    {
        DEBUG_SERIAL.println(F("CALLBACK:  HTTP update process started"));
    }
}

void update_finished()
{
    if (DEBUG)
    {
        DEBUG_SERIAL.println(F("CALLBACK:  HTTP update process finished"));
    }
}

void update_progress(int cur, int total)
{
    if (DEBUG)
    {
        //DEBUG_SERIAL.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
        DEBUG_SERIAL.print(F("CALLBACK:  HTTP update process at"));
        DEBUG_SERIAL.printf(" %d of %d ", cur, total);
        DEBUG_SERIAL.print(F("bytes...\n"));
    }
}

void update_error(int err)
{
    if (DEBUG)
    {
        //DEBUG_SERIAL.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
        DEBUG_SERIAL.print(F("CALLBACK:  HTTP update fatal error code"));
        DEBUG_SERIAL.printf(" %d\n", err);
    }
}

// CALLBACK:  HTTP update fatal error code -101
// HTTP_UPDATE_FAILD Error (-101): Server Did Not Report Size
void otaStart(const char *linkOTA)
{
    if (DEBUG)
    {
        DEBUG_SERIAL.print(F("OTA :: Receiving OTA: "));
        DEBUG_SERIAL.print(linkOTA);
        DEBUG_SERIAL.println(F(" ..."));
    }
    // Create a list of certificates with the server certificate
    BearSSL::X509List otaCert(IRG_Root_X1);
    BearSSL::WiFiClientSecure otaServ;
    // WiFiClient otaServ;
    //  Привязываем корневой сертификат к клиенту OTA Server
    otaServ.setTrustAnchors(&otaCert);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    // Запускаем обновление
    t_httpUpdate_return ret = ESPhttpUpdate.update(otaServ, linkOTA);
    // Анализируем результат
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        if (DEBUG)
        {
            //DEBUG_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            DEBUG_SERIAL.print(F("HTTP_UPDATE_FAILD Error"));
            DEBUG_SERIAL.printf(" (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        }
        break;

    case HTTP_UPDATE_NO_UPDATES:
        if (DEBUG)
        {
            DEBUG_SERIAL.println(F("HTTP_UPDATE_NO_UPDATES"));
        }
        break;

    case HTTP_UPDATE_OK:
        if (DEBUG)
        {
            DEBUG_SERIAL.println(F("HTTP_UPDATE_OK"));
        }
        break;
    };

    if (DEBUG)
    {
        DEBUG_SERIAL.println();
    }
}