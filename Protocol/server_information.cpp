#include "server_information.h"

ServerInformation::ServerInformation(QByteArray &buffer)
{
    QDataStream ds(&buffer, QIODevice::ReadOnly);

    quint16 ident; ds >> ident;
    // Check correct ident
    if (ident != SERVER_INFORMATION)
        throw (BadPacket());
    // Deserealize packet
    ds >> name >> address >> port;
}

QByteArray ServerInformation::serialize()
{
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << SERVER_INFORMATION                // Packet ident
       << name << address << port           // Packet info
       << "SIGN";                           // Packet signature
    return buffer;
}
