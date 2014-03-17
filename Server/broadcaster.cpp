#include "broadcaster.h"
#include <cs_packet.h>

Broadcaster::Broadcaster(QObject *parent)
    : QObject(parent)
{
    t.setInterval(BROADCASTER_TIME_INTERVAL);
    connect(&t, SIGNAL(timeout()), SLOT(sendInformation()));
    t.start();
}

void Broadcaster::setServerInformation(ServerInformation &info)
{
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    // Serialisation
    ds << PacketType::DISCOVER << info;
    // Store serialized packet
    serverPacket = buffer;
}

void Broadcaster::sendInformation()
{
    if (serverPacket.size())
        sock.writeDatagram(serverPacket,
                       QHostAddress::Broadcast, SERVER_INFORMATION_PORT);
}
