// Параметры подключения к WiFi
const char *SSID = "MTSRouter_B769";
const char *PASS = "98797899";

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200
#define DEBUG true

// Номера выводов для подключения реле
const int gpioWiFi = 5; // красный индикатор
const int gpioOnOff = 2; // зеленый индикатор
const int gpioMQTT = 0; // синий индикатор

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

const char lvlRelayOn = 0x1;
const char lvlRelayOff = 0x0;
char lvlRelayFlag = 0x0;

bool first_flag = true; // Первая отправка данных после загрузки скетча
/*
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
*/



