#ifndef RECEIVER_H
#define RECEIVER_H

#include <QBuffer>
#include <QTcpServer>
#include <QAudioOutput>
#include <channel_information.h>

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QHostAddress &address, QObject *parent = 0);

    ChannelInformation getChannel() const
    {
        return channel;
    }

public slots:
    void setVolume(qreal volume);
    void setAudioDevice(QAudioDeviceInfo &device);

private slots:
    void newConnection();
    void sockReadyRead();
    void audioStateChanged(QAudio::State state);

private:
    ChannelInformation channel;
    QTcpSocket *client;
    QTcpServer server;

    QBuffer buffer;
    QAudioFormat format;
    QAudioOutput *audio;
};

#endif // RECEIVER_H
