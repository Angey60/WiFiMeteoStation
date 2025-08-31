#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266httpUpdate.h>
#include <LittleFS.h>

#include <time.h>

// Параметры подключения к WiFi
const char *SSID = "MTSRouter_B769";
const char *PASS = "98797899";

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200
#define DEBUG true

// https://github.com/Angey60/WiFiMeteoStation/blob/main/firmware.bin
// https://angey60meteo.ru/firmware.bin
//
// GitHub.com
/*const char IRG_Root_X1[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIEoTCCBEigAwIBAgIRAKtmhrVie+gFloITMBKGSfUwCgYIKoZIzj0EAwIwgY8x
CzAJBgNVBAYTAkdCMRswGQYDVQQIExJHcmVhdGVyIE1hbmNoZXN0ZXIxEDAOBgNV
BAcTB1NhbGZvcmQxGDAWBgNVBAoTD1NlY3RpZ28gTGltaXRlZDE3MDUGA1UEAxMu
U2VjdGlnbyBFQ0MgRG9tYWluIFZhbGlkYXRpb24gU2VjdXJlIFNlcnZlciBDQTAe
Fw0yNTAyMDUwMDAwMDBaFw0yNjAyMDUyMzU5NTlaMBUxEzARBgNVBAMTCmdpdGh1
Yi5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAAQgNFxG/yzL+CSarvC7L3ep
H5chNnG6wiYYxR5D/Z1J4MxGnIX8KbT5fCgLoyzHXL9v50bdBIq6y4AtN4gN7gbW
o4IC/DCCAvgwHwYDVR0jBBgwFoAU9oUKOxGG4QR9DqoLLNLuzGR7e64wHQYDVR0O
BBYEFFPIf96emE7HTda83quVPjA9PdHIMA4GA1UdDwEB/wQEAwIHgDAMBgNVHRMB
Af8EAjAAMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjBJBgNVHSAEQjBA
MDQGCysGAQQBsjEBAgIHMCUwIwYIKwYBBQUHAgEWF2h0dHBzOi8vc2VjdGlnby5j
b20vQ1BTMAgGBmeBDAECATCBhAYIKwYBBQUHAQEEeDB2ME8GCCsGAQUFBzAChkNo
dHRwOi8vY3J0LnNlY3RpZ28uY29tL1NlY3RpZ29FQ0NEb21haW5WYWxpZGF0aW9u
U2VjdXJlU2VydmVyQ0EuY3J0MCMGCCsGAQUFBzABhhdodHRwOi8vb2NzcC5zZWN0
aWdvLmNvbTCCAX4GCisGAQQB1nkCBAIEggFuBIIBagFoAHUAlpdkv1VYl633Q4do
NwhCd+nwOtX2pPM2bkakPw/KqcYAAAGU02uUSwAABAMARjBEAiA7i6o+LpQjt6Ae
EjltHhs/TiECnHd0xTeer/3vD1xgsAIgYlGwRot+SqEBCs//frx/YHTPwox9QLdy
7GjTLWHfcMAAdwAZhtTHKKpv/roDb3gqTQGRqs4tcjEPrs5dcEEtJUzH1AAAAZTT
a5PtAAAEAwBIMEYCIQDlrInx7J+3MfqgxB2+Fvq3dMlk1qj4chOw/+HkYVfG0AIh
AMT+JKAQfUuIdBGxfryrGrwsOD3pRs1tyAyykdPGRgsTAHYAyzj3FYl8hKFEX1vB
3fvJbvKaWc1HCmkFhbDLFMMUWOcAAAGU02uUJQAABAMARzBFAiEA1GKW92agDFNJ
IYrMH3gaJdXsdIVpUcZOfxH1FksbuLECIFJCfslINhc53Q0TIMJHdcFOW2tgG4tB
A1dL881tXbMnMCUGA1UdEQQeMByCCmdpdGh1Yi5jb22CDnd3dy5naXRodWIuY29t
MAoGCCqGSM49BAMCA0cAMEQCIHGMp27BBBJ1356lCe2WYyzYIp/fAONQM3AkeE/f
ym0sAiBtVfN3YgIZ+neHEfwcRhhz4uDpc8F+tKmtceWJSicMkA==
-----END CERTIFICATE-----
)CERT";*/
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
// Iot Core
static const char ISRG_Root_X2[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
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
/oM/ByVm5+t4D2I381b5EvGCJ4HogA==
-----END PRIVATE KEY-----
)EOF";

// Create a list of certificates with the server certificate
X509List cert(IRG_Root_X1);
BearSSL::X509List certISRG(ISRG_Root_X2);

String firmware_url = "https://angey60meteo.ru/firmware.bin";

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
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  };
  Serial.println("");
}

// put function declarations here:
int myFunction(int, int);

void setup()
{
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
  DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
  while (!DEBUG_SERIAL)
  {
    ;
  }
  delay(500);

  if (DEBUG)
  {
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println(F("Device started ..."));
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.flush();
    delay(500);
  }

  if (!LittleFS.begin())
  {
    if (DEBUG)
    {
      DEBUG_SERIAL.println("An Error has occurred while mounting LittleFS");
      //return;
    }
  }

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  if (DEBUG)
  {
    DEBUG_SERIAL.print("Connecting to WiFi ...");
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    if (DEBUG)
    {
      DEBUG_SERIAL.print(".");
    }
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    if (DEBUG)
    {
      DEBUG_SERIAL.println(" ok");
    }
    // Запускаем обновление
    // otaStart(firmware_url.c_str());
  }
  else
  {
    if (DEBUG)
    {
      DEBUG_SERIAL.println(" no!!!");
    }
  }

  if (DEBUG)
  {
    DEBUG_SERIAL.println();
  }

  delay(1000);
}

void loop()
{
  // put your main code here, to run repeatedly:
  /*
  WiFiClientSecure client;
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED))
  {
    // Корректируем дату и время
    setClock();

    client.setTrustAnchors(&cert);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(client, "https://github.com/Angey60/WiFiMeteoStation/blob/main/firmware.bin"))
    {
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          //String payload = https.getString();
          //Serial.println(payload);
        }
      }
      else
      {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    }
    else
    {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  Serial.println();
  Serial.println("Waiting 2min before the next round...");
  delay(10000);
  */
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}