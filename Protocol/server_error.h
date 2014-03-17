#ifndef SERVER_ERROR_H
#define SERVER_ERROR_H

#include "cs_packet.h"
#include <QString>

struct ServerError
{
    ServerError(QString msg = "")
        : message(msg)
    {}

    QString message;
};

QDataStream &operator >>(QDataStream &ds, ServerError &p);
QDataStream &operator <<(QDataStream &ds, const ServerError &p);

#endif // SERVER_ERROR_H
