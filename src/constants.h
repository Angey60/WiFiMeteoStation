// Параметры подключения к WiFi
const char *SSID = "MTSRouter_B769";
const char *PASS = "98797899";

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200
#define DEBUG true

// Номера выводов для подключения реле
const int gpioRelay = 5;
const int expander_gpioRelay = 2;

const char *mqttserver = "mqtt.cloud.yandex.net";
const char *yandexIoTCoreBrokerId = "a44fnbeb6ejfibpr07j6";
const char *mqttpassword = "Nz;tkfBytrfpbcnf:bpymRjnfEghjuhfvvbcnf@7389";
const int mqttport = 8883;

const String deviceId = "device-iot-wifi_slot"; // unique device id for cloud function
const String commands_01 = String("/yandex-iot-core/" + deviceId + "/commands");

const char *commands = "/yandex-iot-core/#";
const char *events = "warning_light/#";

String firmware_url = "https://angey60meteo.ru/firmware.bin";