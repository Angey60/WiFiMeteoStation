#include <FS.h>
#include <LittleFS.h>
// https://github.com/esp8266/Arduino/tree/master/libraries/LittleFS
/*
  if (!LittleFS.begin())
  {
    DEBUG_SERIAL.println("An Error has occurred while mounting LittleFS");
    // return;
  }

  File file = LittleFS.open("/text.txt", "r");
  if (!file)
  {
    DEBUG_SERIAL.println("Failed to open file for reading");
    // return;
  }

  DEBUG_SERIAL.print("File Content: ");
  while (file.available())
  {
    DEBUG_SERIAL.write(file.read());
  }
  file.close();
  */
