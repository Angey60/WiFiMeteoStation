
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200
#define DEBUG true

// Номера выводов для подключения реле
const int gpioWiFi = 5;   // красный индикатор
const int gpioOnOff = 2;  // зеленый индикатор
const int gpioMQTT = 0;   // синий индикатор

const char gpioSignalOn = 0x1;
const char gpioSignalOff = 0x0;

bool first_flag = true; // Первая отправка данных после загрузки скетча
/*
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
*/




