#ifndef USER_INFORMATION_H
#define USER_INFORMATION_H

#include <QString>

struct UserInformation
{
    UserInformation(QString userName = "")
        : name(userName)
    {}

    QString name;
};

QDataStream &operator >>(QDataStream &ds, UserInformation &p);
QDataStream &operator <<(QDataStream &ds, const UserInformation &p);


#endif // USER_INFORMATION_H
