#ifndef USER_INFORMATION_H
#define USER_INFORMATION_H

#include <QDataStream>

class UserInformation
{
public:
    // Constructior
    UserInformation(QString userName = "")
        : name(userName)
    {}
    // Deserialization constructor
    UserInformation(QDataStream &ds);
    // Packet serialization by data stream
    QByteArray serialize();

    inline QString getName() const
    {
        return name;
    }

private:
    QString name;
};

#endif // USER_INFORMATION_H
