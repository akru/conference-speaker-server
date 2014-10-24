#ifndef RECEIVER_H
#define RECEIVER_H

#include <QUdpSocket>
#include <channel_information.h>

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QHostAddress address, QObject *parent = 0);
    ~Receiver();

    ChannelInformation getChannelInfo() const
    {
        return channel;
    }

    QHostAddress getPeerAddress() const
    {
        return sock.peerAddress();
    }

signals:
    void sampleReceived(QString, QByteArray);

private slots:
    void sockReadyRead();

private:
    QUdpSocket         sock;
    ChannelInformation channel;
};

#endif // RECEIVER_H
