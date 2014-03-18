#include "broadcaster.h"
#include <cs_packet.h>
#include <QJsonDocument>

Broadcaster::Broadcaster(QObject *parent)
    : QObject(parent)
{
    t.setInterval(BROADCASTER_TIME_INTERVAL);
    connect(&t, SIGNAL(timeout()), SLOT(sendInformation()));
    t.start();
}

void Broadcaster::setServerInformation(ServerInformation &info)
{
    QJsonObject packet = info.toJson();
    serverPacket = QJsonDocument(packet).toJson();
    qDebug() << "New server info:" << QJsonDocument(packet).toJson();
}

void Broadcaster::sendInformation()
{
    if (serverPacket.size())
        sock.writeDatagram(serverPacket,
                       QHostAddress::Broadcast, SERVER_INFORMATION_PORT);
}
