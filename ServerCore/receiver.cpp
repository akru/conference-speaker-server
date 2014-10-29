#include "receiver.h"
#include <QHostAddress>

Receiver::Receiver(QHostAddress address, QObject *parent)
    : QObject(parent)
{
    // Bind socket
    if (sock.bind(address))
    {
        channel = ChannelInformation(sock.localAddress().toString(),
                                     sock.localPort());
        qDebug() << "Init channel port:" << sock.localPort();
        connect(&sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
    }
    else
        throw(std::exception());
}

Receiver::~Receiver()
{
    sock.close();
}

void Receiver::sockReadyRead()
{
    QByteArray buf;
    QHostAddress peer;
    // Receive audio sample
    qDebug() << "New data size" << sock.pendingDatagramSize();
    buf.resize(sock.pendingDatagramSize());
    sock.readDatagram(buf.data(), buf.size(), &peer);
    // Play sample
    emit sampleReceived(peer.toString(), buf);
}

