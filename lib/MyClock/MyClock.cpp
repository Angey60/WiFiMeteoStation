#include "MyClock.h"
#include "clock_constants.h"

MyClock::MyClock() {}
MyClock::~MyClock() {}

void MyClock::begin()
{
    // настраиваем часы реального времени
    if (!rtc.begin())
    {
        while (1)
            ;
    }
    if (!rtc.isrunning())
    {
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

// Set time via NTP, as required for x.509 validation
void MyClock::setClock()
{
    // Для работы TLS-соединения нужны корректные дата и время, получаем их с NTP серверов
    configTime("MSK-3", "pool.ntp.org", "time.nist.gov");

    if (CLOCK_DEBUG)
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
            if (CLOCK_DEBUG)
            {
                Serial.println();
                Serial.println(F("Time sync failed!"));
            }
            // return false;
        };
        if (CLOCK_DEBUG)
        {
            Serial.print(F("."));
        }
        delay(500);
    }

    // Время успешно синхронизировано, выводим его в монитор порта
    // Только для целей отладки, этот блок можно исключить
    if (CLOCK_DEBUG)
    {
        Serial.println(F(" ok"));
    }

    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    if (CLOCK_DEBUG)
    {
        Serial.print(F("Current time: "));
        Serial.print(asctime(&timeinfo));
        Serial.println();
    }
    // корректируем время бортовых часов
    rtc.adjust(DateTime(
        2000 + (timeinfo.tm_year - 100),
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec));

    if (CLOCK_DEBUG)
    {
        DateTime now1 = rtc.now();
        Serial.print(F("Date & Time: "));
        Serial.print(now1.year(), DEC);
        Serial.print(F("/"));
        Serial.print(now1.month(), DEC);
        Serial.print(F("/"));
        Serial.print(now1.day(), DEC);
        Serial.print(F(" ("));
        Serial.print(now1.dayOfTheWeek());
        Serial.print(F(") "));
        Serial.print(now1.hour(), DEC);
        Serial.print(F(":"));
        Serial.print(now1.minute(), DEC);
        Serial.print(F(":"));
        Serial.println(now1.second(), DEC);
    }
}

void MyClock::run()
{
}
