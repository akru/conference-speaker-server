#include "discover.h"
#include <cs_packet.h>
#include <QJsonDocument>

Discover::Discover(QObject *parent)
    : QObject(parent)
{
    qDebug() << "sock bind:"
             << sock.bind(SERVER_INFORMATION_PORT);
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
        server = ServerInformation::fromJson(packet);
    } catch (BadPacket) {
        qDebug() << "Discover :: Bad packet:" << readBuffer;
        return;
    }
    emit discovered(server);
}
