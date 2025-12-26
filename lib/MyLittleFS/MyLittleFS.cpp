#include <MyLittleFS.h>
#include "littlefs_constants.h"
#include <FS.h>
#include <LittleFS.h>

MyLittleFS::MyLittleFS() {}
MyLittleFS::~MyLittleFS() {}

bool MyLittleFS::begin()
{
    return LittleFS.begin();
}

void MyLittleFS::end()
{
    LittleFS.end();
}

void MyLittleFS::format()
{
    if (LITTLEFS_DEBUG)
    {
        Serial.println(F("Formatting LittleFS filesystem"));
    }
    LittleFS.format();
}

String MyLittleFS::readFile(const char *path)
{
    if (LITTLEFS_DEBUG)
    {
        Serial.printf("Reading file: %s\n", path);
    }

    String str = "";

    File file = LittleFS.open(path, "r");
    if (!file)
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Failed to open file for reading"));
        }
        return str;
    }

    while (file.available())
    {
        str = str + char(file.read());
    }

    file.close();

    return str;
}

bool MyLittleFS::writeFile(const char *path, const char *message)
{
    if (LITTLEFS_DEBUG)
    {
        Serial.printf("Writing file: %s\n", path);
    }

    bool flag = false;

    File file = LittleFS.open(path, "w");
    if (!file)
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Failed to open file for writing"));
        }
        return flag;
    }

    flag = file.print(message);

    if (flag)
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("File written"));
        }
    }
    else
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Write failed"));
        }
    }

    file.close();

    return flag;
}

bool MyLittleFS::appendFile(const char *path, const char *message)
{
    if (LITTLEFS_DEBUG)
    {
        Serial.printf("Appending to file: %s\n", path);
    }

    bool flag = false;

    File file = LittleFS.open(path, "a");
    if (!file)
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Failed to open file for appending"));
        }
        return flag;
    }

    flag = file.print(message);

    if (flag)
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Message appended"));
        }
    }
    else
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Append failed"));
        }
    }

    file.close();

    return flag;
}

bool MyLittleFS::renameFile(const char *path1, const char *path2)
{
    if (LITTLEFS_DEBUG)
    {
        Serial.printf("Renaming file %s to %s\n", path1, path2);
    }

    bool flag = LittleFS.rename(path1, path2);

    if (flag)
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("File renamed"));
        }
    }
    else
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Rename failed"));
        }
    }

    return flag;
}

bool MyLittleFS::deleteFile(const char *path)
{
    if (LITTLEFS_DEBUG)
    {
        Serial.printf("Deleting file: %s\n", path);
    }

    bool flag = LittleFS.remove(path);

    if (flag)
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("File deleted"));
        }
    }
    else
    {
        if (LITTLEFS_DEBUG)
        {
            Serial.println(F("Delete failed"));
        }
    }

    return flag;
}

bool MyLittleFS::checkFile(const char *path)
{
    if (LITTLEFS_DEBUG)
    {
        Serial.printf("Checking file: %s\n", path);
    }

    bool flag = false;

    File file = LittleFS.open(path, "r");
    if (file)
    {
        flag = true;
    }

    return flag;
}

/*void listDir(const char *dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.print(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm *tmstruct = localtime(&cr);
    Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
}
  */

