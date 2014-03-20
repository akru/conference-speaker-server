#include "broadcaster.h"
#include <cs_packet.h>
#include <QJsonDocument>

Broadcaster::Broadcaster(QObject *parent)
    : QObject(parent)
{
    sock.setSocketOption(QAbstractSocket::MulticastTtlOption, 2);

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
    QHostAddress addr = QHostAddress::Broadcast;
    if (serverPacket.size())
        sock.writeDatagram(serverPacket,
                       addr, SERVER_INFORMATION_PORT);
}
