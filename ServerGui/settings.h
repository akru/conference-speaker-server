#ifndef SETTINGS_H
#define SETTINGS_H

#include <server_information.h>

class Settings
{
public:
    Settings();
    // Setting save method (dumps settings rows to disk)
    void save();
    /* Settings rows */
    // Server information loaded
    ServerInformation info;
    /* Settings rows end */

private:
    QString filename;
};

#endif // SETTINGS_H
