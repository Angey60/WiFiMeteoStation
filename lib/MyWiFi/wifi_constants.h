// Параметры подключения к WiFi
const char *SSID = "MTSRouter_B769";
const char *PASS = "98797899";

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200
#define DEBUG true

const int gpioWiFi = 5; // красный индикатор
const int gpioOnOff = 2; // зеленый индикатор
const int gpioMQTT = 0; // синий индикатор

const char gpioSignalOn = 0x1;
const char gpioSignalOff = 0x0;