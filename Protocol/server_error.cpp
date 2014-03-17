#include "server_error.h"

QDataStream &operator >>(QDataStream &ds, ServerError &p)
{
    return ds >> p.message;
}

QDataStream &operator <<(QDataStream &ds, const ServerError &p)
{
    return ds << p.message;        // Packet info
}

