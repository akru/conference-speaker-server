#include "receiver.h"

#include <QHostAddress>
#include <QAudioOutput>
#include <QTcpSocket>

Receiver::Receiver(QHostAddress address, QObject *parent)
    : QObject(parent),
      client(0),
      audio(0)
{
    // Set up the format, eg.
    format.setSampleSize(16);
    format.setChannelCount(1);
    format.setSampleRate(8000);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        throw(std::exception());
    }
    qDebug() << "Audio output:" << info.deviceName();

    // Open audio device
    audio = new QAudioOutput(info, format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)),
            SLOT(audioStateChanged(QAudio::State)));

    server.setMaxPendingConnections(1); // Set max connentions to only one
    if (server.listen(address))
    {
        channel = ChannelInformation(server.serverAddress().toString(),
                                     server.serverPort());
        qDebug() << "Init channel port:" << server.serverPort();
        connect(&server, SIGNAL(newConnection()), SLOT(newConnection()));
    }
    else
        throw(std::exception());
}

void Receiver::audioStateChanged(QAudio::State state)
{
    qDebug() << "New audio state:" << state;
}

void Receiver::newConnection()
{
    client = server.nextPendingConnection();
    qDebug() << "New connection from" << client->peerAddress().toString();

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
    Q_ASSERT(audio);
    audio->setVolume(volume);
}
