#ifndef RECEIVER_H
#define RECEIVER_H

#include <QUdpSocket>
#include <QAudioOutput>
#include <channel_information.h>
#include <filter_thread.h>

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
    void audioAmpUpdated(int amplitude);
    void sampleReceived(QByteArray data);

public slots:
    void setVolume(qreal volume);

private slots:
    void audioStateChanged(QAudio::State state);
    void sockReadyRead();
    void play(QByteArray data);

private:
    QUdpSocket sock;
    QIODevice *buffer;
    ChannelInformation channel;

    QAudioFormat format;
    QAudioOutput *audio;

    FIlterThread filters;

    void ampAnalyze(QByteArray &sample);
};

#endif // RECEIVER_H
