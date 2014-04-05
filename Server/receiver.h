#ifndef RECEIVER_H
#define RECEIVER_H

#include <QUdpSocket>
#include <QAudioOutput>
#include <channel_information.h>
#include <bandpass_filter.h>

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

public slots:
    void setVolume(qreal volume);

private slots:
    void audioStateChanged(QAudio::State state);
    void sockReadyRead();

private:
    QUdpSocket sock;
    QIODevice *buffer;
    ChannelInformation channel;

    BandpassFilter filter;
    QAudioFormat format;
    QAudioOutput *audio;

    void ampAnalyze(QByteArray &sample);
};

#endif // RECEIVER_H
