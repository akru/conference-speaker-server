#include "discover.h"
#include <cs_packet.h>
#include <QJsonDocument>
#include <QNetworkInterface>

Discover::Discover(QObject *parent)
    : QObject(parent)
{
    QHostAddress addr = QNetworkInterface::allInterfaces()[0]
            .addressEntries()[0].broadcast();
    qDebug() << "sock bind:"
             << sock.bind(addr, SERVER_DISCOVER_PORT);
    connect(&sock, SIGNAL(readyRead()), SLOT(readPacket()));
}

void Discover::readPacket()
{
    ServerInformation server;
    QByteArray readBuffer;
    QJsonObject packet;
    // Resize buffer by pending data
    readBuffer.resize(sock.pendingDatagramSize());
    // Receive data
    sock.readDatagram(readBuffer.data(), readBuffer.size());
    // Parse packet
    try {
        packet = QJsonDocument::fromJson(readBuffer).object();
        server = ServerInformation::fromJson(packet.take("info").toObject());
    } catch (BadPacket) {
        qDebug() << "Discover :: Bad packet:" << readBuffer;
        return;
    }
    emit discovered(server);
}
