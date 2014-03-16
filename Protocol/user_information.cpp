#include "user_information.h"

UserInformation::UserInformation(QDataStream &ds)
{
    // Restore custom values
    ds >> name;
}

QByteArray UserInformation::serialize()
{
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << name;                             // Packet value
    return buffer;
}
