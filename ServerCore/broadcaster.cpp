#include "broadcaster.h"
#include <cs_packet.h>
#include <QJsonDocument>
#include <QNetworkInterface>

Broadcaster::Broadcaster(QObject *parent)
    : QObject(parent)
{
    sock.setSocketOption(QAbstractSocket::MulticastTtlOption, 2);
    // Find all broadcast addreses in all interfaces
    foreach (const QNetworkInterface &iface,
             QNetworkInterface::allInterfaces()) {
        foreach (const QNetworkAddressEntry &addr, iface.addressEntries()) {
            addreses.append(addr.broadcast());
        }
    }

    t.setInterval(BROADCASTER_TIME_INTERVAL);
    connect(&t, SIGNAL(timeout()), SLOT(sendInformation()));
    t.start();
}

void Broadcaster::setServerInformation(ServerInformation info)
{
    QJsonObject packet = info.toJson();
    serverPacket = QJsonDocument(packet).toJson();
    qDebug() << "New server info:" << QJsonDocument(packet).toJson();
}

void Broadcaster::sendInformation()
{
    foreach (const QHostAddress &addr, addreses) {
        if (serverPacket.size())
            sock.writeDatagram(serverPacket,
                                addr, SERVER_INFORMATION_PORT);
    }
}
