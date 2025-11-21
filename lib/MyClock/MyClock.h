#include <time.h>
#include <RTClib.h>

#ifndef MyClock_H
#define MyClock_H

#include <Arduino.h>
#include <avr/pgmspace.h>

class MyClock
{
public:
    MyClock();
    ~MyClock();

    void begin();
    void run();

    void setClock();

protected:

private:
    // создаём объект для работы с часами реального времени
    RTC_DS1307 rtc;
};

#endif // MyClock_H