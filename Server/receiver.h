#ifndef RECEIVER_H
#define RECEIVER_H

#include <QBuffer>
#include <QTcpSocket>
#include <QAudioOutput>
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

public slots:
    void setVolume(qreal volume);
    void audioStateChanged(QAudio::State state);

private slots:
    void sockReadyRead();

private:
    ChannelInformation channel;
    QTcpSocket sock;

    QBuffer buffer;
    QAudioFormat format;
    QAudioOutput *audio;
};

#endif // RECEIVER_H
