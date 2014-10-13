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
    ~Receiver();

    ChannelInformation getChannelInfo() const
    {
        return channel;
    }

    QHostAddress getPeerAddress() const
    {
        return sock.peerAddress();
    }

signals:
    void audioAmpUpdated(int);
    void sampleReceived(QByteArray);

public slots:
    void setVolume(qreal volume)
    {
        speaker.setVolume(volume);
    }
    void reloadFilterSettings()
    {
        speaker.reloadFilterSettings();
    }

private slots:
    void sockReadyRead();
    void updateAmp(int amplitude)
    {
        emit audioAmpUpdated(amplitude);
    }

private:
    QUdpSocket         sock;
    Speaker            speaker;
    ChannelInformation channel;
};

#endif // RECEIVER_H
