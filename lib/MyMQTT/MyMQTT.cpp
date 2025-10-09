#include "MyMQTT.h"
#include "constants.h"
#include "mqtt_constants.h"

// Iot Core
static const char ISRG_Root_X12[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIFGTCCAwGgAwIBAgIQJMM7ZIy2SYxCBgK7WcFwnjANBgkqhkiG9w0BAQ0FADAf
MR0wGwYDVQQDExRZYW5kZXhJbnRlcm5hbFJvb3RDQTAeFw0xMzAyMTExMzQxNDNa
Fw0zMzAyMTExMzUxNDJaMB8xHTAbBgNVBAMTFFlhbmRleEludGVybmFsUm9vdENB
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAgb4xoQjBQ7oEFk8EHVGy
1pDEmPWw0Wgw5nX9RM7LL2xQWyUuEq+Lf9Dgh+O725aZ9+SO2oEs47DHHt81/fne
5N6xOftRrCpy8hGtUR/A3bvjnQgjs+zdXvcO9cTuuzzPTFSts/iZATZsAruiepMx
SGj9S1fGwvYws/yiXWNoNBz4Tu1Tlp0g+5fp/ADjnxc6DqNk6w01mJRDbx+6rlBO
aIH2tQmJXDVoFdrhmBK9qOfjxWlIYGy83TnrvdXwi5mKTMtpEREMgyNLX75UjpvO
NkZgBvEXPQq+g91wBGsWIE2sYlguXiBniQgAJOyRuSdTxcJoG8tZkLDPRi5RouWY
gxXr13edn1TRDGco2hkdtSUBlajBMSvAq+H0hkslzWD/R+BXkn9dh0/DFnxVt4XU
5JbFyd/sKV/rF4Vygfw9ssh1ZIWdqkfZ2QXOZ2gH4AEeoN/9vEfUPwqPVzL0XEZK
r4s2WjU9mE5tHrVsQOZ80wnvYHYi2JHbl0hr5ghs4RIyJwx6LEEnj2tzMFec4f7o
dQeSsZpgRJmpvpAfRTxhIRjZBrKxnMytedAkUPguBQwjVCn7+EaKiJfpu42JG8Mm
+/dHi+Q9Tc+0tX5pKOIpQMlMxMHw8MfPmUjC3AAd9lsmCtuybYoeN2IRdbzzchJ8
l1ZuoI3gH7pcIeElfVSqSBkCAwEAAaNRME8wCwYDVR0PBAQDAgGGMA8GA1UdEwEB
/wQFMAMBAf8wHQYDVR0OBBYEFKu5xf+h7+ZTHTM5IoTRdtQ3Ti1qMBAGCSsGAQQB
gjcVAQQDAgEAMA0GCSqGSIb3DQEBDQUAA4ICAQAVpyJ1qLjqRLC34F1UXkC3vxpO
nV6WgzpzA+DUNog4Y6RhTnh0Bsir+I+FTl0zFCm7JpT/3NP9VjfEitMkHehmHhQK
c7cIBZSF62K477OTvLz+9ku2O/bGTtYv9fAvR4BmzFfyPDoAKOjJSghD1p/7El+1
eSjvcUBzLnBUtxO/iYXRNo7B3+1qo4F5Hz7rPRLI0UWW/0UAfVCO2fFtyF6C1iEY
/q0Ldbf3YIaMkf2WgGhnX9yH/8OiIij2r0LVNHS811apyycjep8y/NkG4q1Z9jEi
VEX3P6NEL8dWtXQlvlNGMcfDT3lmB+tS32CPEUwce/Ble646rukbERRwFfxXojpf
C6ium+LtJc7qnK6ygnYF4D6mz4H+3WaxJd1S1hGQxOb/3WVw63tZFnN62F6/nc5g
6T44Yb7ND6y3nVcygLpbQsws6HsjX65CoSjrrPn0YhKxNBscF7M7tLTW/5LK9uhk
yjRCkJ0YagpeLxfV1l1ZJZaTPZvY9+ylHnWHhzlq0FzcrooSSsp4i44DB2K7O2ID
87leymZkKUY6PMDa4GkDJx0dG4UXDhRETMf+NkYgtLJ+UIzMNskwVDcxO4kVL+Hi
Pj78bnC5yCw8P5YylR45LdxLzLO68unoXOyFz1etGXzszw8lJI9LNubYxk77mK8H
LpuQKbSbIERsmR+QqQ==
-----END CERTIFICATE-----
)CERT";

BearSSL::X509List mqttCert(ISRG_Root_X12);
BearSSL::WiFiClientSecure mqttServ;
//PubSubClient client(mqttServ);

MyMQTT::MyMQTT() {}
MyMQTT::~MyMQTT() {}

void MyMQTT::begin()
{
    client = PubSubClient(mqttServ);
}

bool MyMQTT::connect()
{
    if (!client.connected())
    {
        if (DEBUG)
        {
            DEBUG_SERIAL.print(F("Connecting to Yandex IoT Core as"));
            DEBUG_SERIAL.print(yandexIoTCoreBrokerId);
            DEBUG_SERIAL.print(F(" ..."));
        }

        // Привязываем корневой сертификат к клиенту Yandex Iot Core
        mqttServ.setTrustAnchors(&mqttCert);
        // Настраиваем MQTT клиент
        client.setServer(mqttserver, mqttport);
        //client.setCallback(callback); 
        client.setCallback(callback); 
        client.setBufferSize(1024);
        client.setKeepAlive(15);

        while (!client.connected())
        {
            if (client.connect("angey60_Esp8266Client_broker", yandexIoTCoreBrokerId, mqttpassword))
            {
                if (DEBUG)
                {
                    DEBUG_SERIAL.println(F(" ok"));
                }
            }
            else
            {
                if (DEBUG)
                {
                    DEBUG_SERIAL.print(F("."));
                }
                delay(500);
            }
        }

        if (client.connected())
        {
            if (DEBUG)
            {
                DEBUG_SERIAL.print(F("Subscribe to: "));
                DEBUG_SERIAL.print(commands);
                DEBUG_SERIAL.println(F("\r\n"));
            }
            //
            client.subscribe(commands.c_str());
        }
        else
        {
            if (DEBUG)
            {
                DEBUG_SERIAL.println(F("Connection to the mqtt broker could not be established!"));
            }
        }
    }

    return client.connected();
    
}

bool MyMQTT::gpio_status()
{
    return client.connected();
    // индикатор MQTT
    //expander.digitalWrite(gpioMQTT, flag);
}

bool MyMQTT::isConnected()
{
    return client.connected();
}

void MyMQTT::disconnect()
{
    if (client.connected())
    {
        client.disconnect();
    };
}

void MyMQTT::run()
{
}

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

    if (command == "1")
    {
        //expander.digitalWrite(gpioMQTT, lvlRelayOn);
        lvlRelayFlag = 0x1;
        return;
    }

    if (command == "0")
    {
        //expander.digitalWrite(gpioMQTT, lvlRelayOff);
        lvlRelayFlag = 0x0;
        return;
    }

    if (command == "7") // обновление
    {
        // Корректируем дату и время
        //setClock();
        // Закрываем соединение с MQTT-брокером
        //mqtt_disconnect();
        //delay(1000);
        //otaStart(firmware_url.c_str());
        return;
    }
}


