#include "receiver.h"
#include <QHostAddress>

Receiver::Receiver(QObject *parent)
    : QObject(parent)
{
    if (sock.bind())
    {
        qDebug() << "Open channel:" << channel.toJson();
        channel = ChannelInformation(sock.localPort());
    }
    else
        throw(std::exception());
}
