
#include "MyOTA.h"
#include "ota_constants.h"

// sweb.ru
const char IRG_Root_X1[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)CERT";

// Create a list of certificates with the server certificate
BearSSL::X509List otaCert(IRG_Root_X1);
BearSSL::WiFiClientSecure otaServ;

MyOTA::MyOTA() {}
MyOTA::~MyOTA() {}

String linkOTA = "https://angey60meteo.ru/firmware.bin";

void update_started();
void update_finished();
void update_progress(int cur, int total);
void update_error(int err);

void MyOTA::begin()
{
    // Привязываем корневой сертификат к клиенту OTA Server
    otaServ.setTrustAnchors(&otaCert);

    if (OTA_DEBUG)
    {
        Serial.print(F("OTA :: Receiving OTA: "));
        Serial.print(linkOTA);
        Serial.println(F(" ..."));
    }
    
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
        if (OTA_DEBUG)
        {
            //Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            Serial.print(F("HTTP_UPDATE_FAILD Error"));
            Serial.printf(" (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        }
        break;

    case HTTP_UPDATE_NO_UPDATES:
        if (OTA_DEBUG)
        {
            Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
        }
        break;

    case HTTP_UPDATE_OK:
        if (OTA_DEBUG)
        {
            Serial.println(F("HTTP_UPDATE_OK"));
        }
        break;
    };

    if (OTA_DEBUG)
    {
        Serial.println();
    }
}

void update_started()
{
    if (OTA_DEBUG)
    {
        Serial.println(F("CALLBACK:  HTTP update process started"));
    }
}

void update_finished()
{
    if (OTA_DEBUG)
    {
        Serial.println(F("CALLBACK:  HTTP update process finished"));
    }
}

void update_progress(int cur, int total)
{
    if (OTA_DEBUG)
    {
        //Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
        Serial.print(F("CALLBACK:  HTTP update process at"));
        Serial.printf(" %d of %d ", cur, total);
        Serial.print(F("bytes...\n"));
    }
}

void update_error(int err)
{
    if (OTA_DEBUG)
    {
        //Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
        Serial.print(F("CALLBACK:  HTTP update fatal error code"));
        Serial.printf(" %d\n", err);
    }
}
