#include "server_information.h"

QDataStream &operator >>(QDataStream &ds, ServerInformation &p)
{
    QString signature; // TODO: signature checking
    return ds >> p.name >> p.address >> p.port >> signature;
}

QDataStream &operator <<(QDataStream &ds, const ServerInformation &p)
{
    return ds << p.name << p.address << p.port  // Packet info
           << QString("SIGN");                  // Packet signature
}
