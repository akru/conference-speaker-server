#include "receiver.h"

#include <QHostAddress>


Receiver::Receiver(QHostAddress address, QObject *parent)
    : QObject(parent)
{
    if (sock.bind(address))
    {
        channel = ChannelInformation(sock.localAddress().toString(),
                                     sock.localPort());
        qDebug() << "Init channel port:" << sock.localPort();
        connect(&sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
    }
    else
        throw(std::exception());
    // Check speaker is working
    if (speaker.isDisabled())
        throw(std::exception());
    // Connects with speaker
    connect(&speaker, SIGNAL(audioAmpUpdated(int)), SLOT(updateAmp(int)));
}

void Receiver::sockReadyRead()
{
    QByteArray buf;
    // Receive audio sample
    qDebug() << "New data size" << sock.pendingDatagramSize();
    buf.resize(sock.pendingDatagramSize());
    sock.readDatagram(buf.data(), buf.size());
    // Play sample
    speaker.play(buf);
}

