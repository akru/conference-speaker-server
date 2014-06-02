#ifndef RECEIVER_H
#define RECEIVER_H

#include <QUdpSocket>
#include <channel_information.h>
#include <speaker.h>

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QHostAddress address, QObject *parent = 0);

    ChannelInformation getChannelInfo() const
    {
        return channel;
    }

    QHostAddress getPeerAddress() const
    {
        return sock.peerAddress();
    }

signals:
    void audioAmpUpdated(int amplitude);

public slots:
    void setVolume(qreal volume)
    {
        speaker.setVolume(volume);
    }

private slots:
    void sockReadyRead();
    void updateAmp(int amplitude)
    {
        emit audioAmpUpdated(amplitude);
    }

private:
    QUdpSocket sock;
    QIODevice *buffer;
    Speaker speaker;
    ChannelInformation channel;
};

#endif // RECEIVER_H
