#ifndef MyBLE_H
#define MyBLE_H

#include <Arduino.h>
#include <CustomClass.h>

class MyBLE : public CustomClass
{
public:
    MyBLE();
    ~MyBLE();
    void begin(uint32_t baud);

protected:
private:
};

#endif // MyBLE_H