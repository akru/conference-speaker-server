#ifndef RECEIVER_H
#define RECEIVER_H

#include <QUdpSocket>
#include <QAudioOutput>
#include <channel_information.h>

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QHostAddress address, QObject *parent = 0);
    ~Receiver()
    {
        delete audio;
    }

    ChannelInformation getChannelInfo() const
    {
        return channel;
    }

    QHostAddress getPeerAddress() const
    {
        return sock.peerAddress();
    }

signals:
    void connected(Receiver *);

public slots:
    void setVolume(qreal volume);

private slots:
    void audioStateChanged(QAudio::State state);

private:
    QUdpSocket sock;
    ChannelInformation channel;

    QAudioFormat format;
    QAudioOutput *audio;
};

#endif // RECEIVER_H
