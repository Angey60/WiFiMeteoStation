#include "CustomClass.h"

CustomClass::CustomClass() {}
CustomClass::~CustomClass() {}

void CustomClass::begin()
{
}

void CustomClass::run()
{
}

void CustomClass::custom_delay(unsigned long duration)
{
    unsigned long start = millis();
    while (start + duration > millis())
    {
        sqrt(4700);
    }
}