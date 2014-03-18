#ifndef USER_INFORMATION_H
#define USER_INFORMATION_H

#include <QJsonObject>

struct UserInformation
{
    UserInformation(QString userName = "Anonymous")
        : name(userName)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static UserInformation fromJson(const QJsonObject &json);

    QString name;
};

#endif // USER_INFORMATION_H
