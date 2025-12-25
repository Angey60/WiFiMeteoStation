#include "MyBLE.h"
#include "ble_constants.h"

MyBLE::MyBLE() {}
MyBLE::~MyBLE() {}

void MyBLE::begin(uint32_t baud)
{
    ble_serial.begin(baud);
}