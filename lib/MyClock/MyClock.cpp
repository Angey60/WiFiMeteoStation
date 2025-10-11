#include "MyClock.h"

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

void MyClock::run()
{
}
