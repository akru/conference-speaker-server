#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <channel_information.h>

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QObject *parent = 0);

    ChannelInformation getChannel() const
    {
        return channel;
    }

private:
    ChannelInformation channel;
};

#endif // RECEIVER_H
