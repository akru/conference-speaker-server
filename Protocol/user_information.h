#ifndef USER_INFORMATION_H
#define USER_INFORMATION_H

#include <QJsonObject>

struct UserInformation
{
    UserInformation(QString userName = "Unknown",
                    QString userCompany = "Unknown",
                    QString userTitle = "Unknown")
        : name(userName),
          company(userCompany),
          title(userTitle)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static UserInformation fromJson(const QJsonObject &json);

    QString name;
    QString company;
    QString title;
};

#endif // USER_INFORMATION_H
