#include "broadcaster.h"
#include <cs_packet.h>
#include <QNetworkInterface>
#include <QTimer>
#include <QUuid>

Broadcaster::Broadcaster(QObject *parent)
    : QObject(parent), timer(new QTimer(this))
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
    connect(timer, SIGNAL(timeout()), SLOT(sendInformation()));
    timer->setInterval(timeInterval);
}

void Broadcaster::start()
{
    timer->start();
}

void Broadcaster::stop()
{
    timer->stop();
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

void Broadcaster::setDocumentsInformation(DocumentsInformation docs)
{
    // Update message
    message.insert("documents", docs.toJson());
    // Update cache
    packetCache = QJsonDocument(message).toJson();
    qDebug() << "New broadcast message:" << packetCache;
}

void Broadcaster::sendInformation()
{
    foreach (const QHostAddress &addr, addreses) {
        if (packetCache.size())
            sock.writeDatagram(packetCache, addr, discoverPort);
    }
}
