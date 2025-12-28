#include "MyBLE.h"
#include <SoftwareSerial.h>
#include "ble_constants.h"

SoftwareSerial ble_serial(BLE_TX, BLE_RX);
#define BLE_SERIAL ble_serial

MyBLE::MyBLE() {}
MyBLE::~MyBLE() {}

void MyBLE::begin(uint32_t baud)
{
    BLE_SERIAL.begin(baud);

    if (BLE_DEBUG)
    {
        Serial.println(F("Starting BLE initialization"));
    }

    while (!BLE_SERIAL)
    {
        delay(100);
    }

    if (BLE_SERIAL)
    {
        if (BLE_DEBUG)
        {
            Serial.println(F("End BLE initialization"));
        }
    }
}

size_t MyBLE::write(const char *str)
{
    // передаем в BLE
    return BLE_SERIAL.write(str);
}

bool MyBLE::gpio_status()
{
    return BLE_SERIAL;
}


