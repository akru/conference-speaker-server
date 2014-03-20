#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QTcpSocket>
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

private:
    QTcpSocket sock;
    QAudioDecoder *decoder;
};

#endif // TRANSMITTER_H
