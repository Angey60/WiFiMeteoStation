#ifndef MyLittleFS_H
#define MyLittleFS_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <CustomClass.h>

class MyLittleFS : public CustomClass
{
public:
    MyLittleFS();
    ~MyLittleFS();

    bool begin();
    void end();
    void format();
    String readFile(const char *path);
    bool writeFile(const char *path, const char *message);
    bool appendFile(const char *path, const char *message);
    bool renameFile(const char *path1, const char *path2);
    bool deleteFile(const char *path);
    bool checkFile(const char *path);

protected:
private:
};

#endif // MyLittleFS_H