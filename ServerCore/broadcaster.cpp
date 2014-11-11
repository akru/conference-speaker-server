#include "broadcaster.h"
#include <cs_packet.h>
#include <QNetworkInterface>
#include <QTimer>
#include <QUuid>

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
    // Set server UUID
    message.insert("uuid", QUuid::createUuid().toString());
    // Setup broadcasting interval
    QTimer *t = new QTimer(this);
    t->setInterval(BROADCASTER_TIME_INTERVAL);
    connect(t, SIGNAL(timeout()), SLOT(sendInformation()));
    t->start();
}

void Broadcaster::setServerInformation(ServerInformation info)
{
    // Update message
    message.insert("info", info.toJson());
    // Update cache
    packetCache = QJsonDocument(message).toJson();
    qDebug() << "New broadcast message:" << packetCache;
}

void Broadcaster::setVotingInvite(VotingInvite invite)
{
    // Update message
    message.insert("vote", invite.toJson());
    // Update cache
    packetCache = QJsonDocument(message).toJson();
    qDebug() << "New broadcast message:" << packetCache;
}

void Broadcaster::unsetVotingInvite()
{
    // Update message
    message.remove("vote");
    // Update cache
    packetCache = QJsonDocument(message).toJson();
    qDebug() << "New broadcast message:" << packetCache;
}

void Broadcaster::setAppInfo(AppInformation info)
{
    // Update message
    message.insert("app", info.toJson());
    // Update cache
    packetCache = QJsonDocument(message).toJson();
    qDebug() << "New broadcast message:" << packetCache;
}

void Broadcaster::sendInformation()
{

    foreach (const QHostAddress &addr, addreses) {
        if (packetCache.size())
            sock.writeDatagram(packetCache, addr, SERVER_DISCOVER_PORT);
    }
}
