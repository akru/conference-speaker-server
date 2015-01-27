#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QUdpSocket>
#include <QAudioDecoder>
#include <channel_information.h>

class Transmitter : public QObject
{
    Q_OBJECT
public:
    explicit Transmitter(ChannelInformation channel,
                         QObject *parent = 0);

signals:

public slots:

private slots:
    void connected();
    void readBuffer();

    void sendData();

private:
    ChannelInformation ch;
    QUdpSocket sock;
    QAudioDecoder *decoder;
    QList<QByteArray> data;
};

#endif // TRANSMITTER_H
