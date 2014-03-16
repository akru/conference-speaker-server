#ifndef SERVERINFORMATION_H
#define SERVERINFORMATION_H

#include <QDataStream>
#include "control_packet.h"

// Packet ident
const quint16 SERVER_INFORMATION      = 0xFEDC;
const quint16 SERVER_INFORMATION_PORT = 35000;

class ServerInformation
{
public:
    // Constructor by params
    ServerInformation(QString serverName, QString serverAddr, quint16 port)
        : name(serverName),
          address(serverAddr),
          port(port)
    {}
    // Deserialization constructor
    ServerInformation(QByteArray &buffer);

    // Packet serialization by data stream
    QByteArray serialize();

    inline QString getName() const
    {
        return name;
    }
    inline QString getAddress() const
    {
        return address;
    }
    inline quint16 getPort() const
    {
        return port;
    }

private:
    // Packet value
    QString name;
    QString address;
    quint16 port;
};

#endif // SERVERINFORMATION_H
