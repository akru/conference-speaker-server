#include "receiver.h"

#include <QHostAddress>
#include <QAudioOutput>
#include <QTcpSocket>

Receiver::Receiver(QHostAddress address, QObject *parent)
    : QObject(parent)
{
    if (server.listen(address))
    {
        channel = ChannelInformation(server.serverAddress().toString(),
                                     server.serverPort());
        qDebug() << "Init channel port:" << server.serverPort();
        connect(&server, SIGNAL(newConnection()), SLOT(newConnection()));
        server.setMaxPendingConnections(1);
    }
    else
        throw(std::exception());

    // Set up the format, eg.
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }
    qDebug() << "Audio output:" << info.deviceName();

    // Open audio device
    audio = new QAudioOutput(info, format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)),
            SLOT(audioStateChanged(QAudio::State)));
}

void Receiver::audioStateChanged(QAudio::State state)
{
    qDebug() << "New audio state:" << state;
}

void Receiver::newConnection()
{
    QTcpSocket *client = server.nextPendingConnection();
    peerAddress = client->peerAddress();
    qDebug() << "New connection from" << peerAddress.toString();

    // Set low dealy option and connect disconneced handler
    client->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(client, SIGNAL(disconnected()), SLOT(disconnected()));
    emit connected(this);

    // Start playing channel
    audio->start(client);
}

void Receiver::disconnected()
{
    emit disconnected(this);
}

void Receiver::setVolume(qreal volume)
{
    qDebug() << "Set volume:" << volume;
    audio->setVolume(volume);
}
