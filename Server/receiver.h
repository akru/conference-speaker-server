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

public slots:
    void setVolume(qreal volume);

private slots:
    void audioStateChanged(QAudio::State state);
    void sockReadyRead();

private:
    QUdpSocket sock;
    QIODevice *buffer;
    ChannelInformation channel;

    QAudioFormat format;
    QAudioOutput *audio;
};

#endif // RECEIVER_H
