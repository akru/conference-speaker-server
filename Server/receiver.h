#ifndef RECEIVER_H
#define RECEIVER_H

#include <QTcpServer>
#include <QAudioOutput>
#include <channel_information.h>

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
        return peerAddress;
    }

signals:
    void connected(Receiver *);
    void disconnected(Receiver *);

public slots:
    void setVolume(qreal volume);

private slots:
    void audioStateChanged(QAudio::State state);
    void newConnection();
    void disconnected();

private:
    QTcpServer  server;
    ChannelInformation channel;

    QHostAddress peerAddress;

    QAudioFormat format;
    QAudioOutput *audio;
};

#endif // RECEIVER_H
