#ifndef RECEIVER_H
#define RECEIVER_H

#include <QUdpSocket>
#include <channel_information.h>
#include "user.h"

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(const QHostAddress &address,
                      QObject *parent = 0);
    ~Receiver();

    inline const ChannelInformation & getChannelInfo() const
    { return channelInfo; }

signals:
    void sampleReceived(User*, QByteArray);

private slots:
    void sockReadyRead();

private:
    QUdpSocket         sock;
    ChannelInformation channelInfo;
};

#endif // RECEIVER_H
