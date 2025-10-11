#include <Arduino.h>
const char *mqttserver = "mqtt.cloud.yandex.net";
const char *yandexIoTCoreBrokerId = "a44fnbeb6ejfibpr07j6";
const char *mqttpassword = "Nz;tkfBytrfpbcnf:bpymRjnfEghjuhfvvbcnf@7389";
const int mqttport = 8883;
// /yandex-iot-core/device-iot-wifi_slot/commands
const String deviceId = "device-iot-wifi_slot"; 
const String commands = String("/yandex-iot-core/" + deviceId + "/commands");
// Топики для публикации температуры и влажности
const char *mqttTopicParamerters = "/yandex-iot-core/parameter_values";
const bool mqttSensorsRetained = false;

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200
#define DEBUG true
