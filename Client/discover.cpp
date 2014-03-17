#include "discover.h"
#include <cs_packet.h>

Discover::Discover(QObject *parent)
    : QObject(parent)
{
    sock.bind(QHostAddress::Broadcast, SERVER_INFORMATION_PORT);
    connect(&sock, SIGNAL(readyRead()), SLOT(readPacket()));
}

void Discover::readPacket()
{
    quint8 packetType;
    ServerInformation server;
    QByteArray readBuffer;
    QDataStream ds(&readBuffer, QIODevice::ReadOnly);
    // Resize buffer by pending data
    readBuffer.resize(sock.pendingDatagramSize());
    // Receive data
    sock.readDatagram(readBuffer.data(), readBuffer.size());
    // Parse packet
    ds >> packetType;
    // Check packet type
    if (packetType != PacketType::DISCOVER)
        qDebug() << "Discover :: Bad packet type";
    else
    {
        ds >> server;
        emit discovered(server);
    }
}
