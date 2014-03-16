#include "discover.h"

Discover::Discover(QObject *parent)
    : QObject(parent)
{
    sock.bind(QHostAddress::Broadcast, SERVER_INFORMATION_PORT);
    connect(&sock, SIGNAL(readyRead()), SLOT(readPacket()));
}

void Discover::readPacket()
{
    QByteArray readBuffer;
    // Resize buffer by pending data
    readBuffer.resize(sock.pendingDatagramSize());
    // Receive data
    sock.readDatagram(readBuffer.data(), readBuffer.size());
    try {
        ServerInformation server(readBuffer);
        emit discovered(server);
    }
    catch (BadPacket)
    {
        qDebug() << "Discover :: Bad packet exception";
    }
}
