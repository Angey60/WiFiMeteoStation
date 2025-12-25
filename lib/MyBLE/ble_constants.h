#include <SoftwareSerial.h>

#define BLE_RX A0
#define BLE_TX A1
SoftwareSerial ble_serial(BLE_RX, BLE_TX);
#define BLE_SERIAL ble_serial