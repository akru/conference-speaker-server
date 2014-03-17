#ifndef SERVERINFORMATION_H
#define SERVERINFORMATION_H

#include <QDataStream>

// Packet ident
const quint16 SERVER_INFORMATION_PORT = 35000;

struct ServerInformation
{
    // Constructor by params
    ServerInformation(QString serverName ="",
                      QString serverAddr = "",
                      quint16 port = 0)
        : name(serverName),
          address(serverAddr),
          port(port)
    {}

    QString name;
    QString address;
    quint16 port;
};

QDataStream &operator >>(QDataStream &ds, ServerInformation &p);
QDataStream &operator <<(QDataStream &ds, const ServerInformation &p);

#endif // SERVERINFORMATION_H
