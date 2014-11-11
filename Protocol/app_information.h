#ifndef APP_INFORMATION_H
#define APP_INFORMATION_H

#include <QJsonObject>

struct AppInformation
{
    AppInformation(const QString &address, int version=1);

    // Json serializer
    QJsonObject toJson() const;

    QString uriAndroid;
    QString uriIos;
    int     version;
};

#endif // APP_INFORMATION_H
