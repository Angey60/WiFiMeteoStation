#include <time.h>
#include <RTClib.h>

#ifndef CustomClass_H
#define CustomClass_H

#include <Arduino.h>
#include <avr/pgmspace.h>

class CustomClass
{
public:
    CustomClass();
    ~CustomClass();
    void begin();
    void run();

protected:
    void custom_delay(unsigned long duration);
};

#endif // CustomClass_H