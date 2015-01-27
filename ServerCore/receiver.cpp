#include "receiver.h"
#include <QHostAddress>

Receiver::Receiver(const QHostAddress &address,
                   QObject *parent)
    : QObject(parent)
{
    // Bind socket
    if (sock.bind(address))
    {
        channelInfo = ChannelInformation(
                    sock.localAddress().toString(),
                    sock.localPort());
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
    // Emit sample by user
    User *user = qobject_cast<User *>(parent());
    // Cast check
    if (!user) return;
    // Sender simple check
    if (user->getID() == User::ID(peer))
        emit sampleReceived(user, buf);
}

