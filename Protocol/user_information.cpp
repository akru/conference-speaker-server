#include "user_information.h"

QDataStream &operator >>(QDataStream &ds, UserInformation &p)
{
    return ds >> p.name;
}

QDataStream &operator <<(QDataStream &ds, const UserInformation &p)
{
    return ds << p.name; // Packet info
}
