#include "settings.h"

#include <QCoreApplication>
#include <QSettings>

Settings::Settings()
    : filename(QCoreApplication::applicationDirPath() + "/settings.ini")
{
    QSettings s(filename, QSettings::IniFormat);
    info.name    = s.value("conference-name", "").toString();
    info.address = s.value("ip-address", "").toString();
}

void Settings::save()
{
    QSettings s(filename, QSettings::IniFormat);
    s.setValue("conference-name", info.name);
    s.setValue("ip-address", info.address);
}
