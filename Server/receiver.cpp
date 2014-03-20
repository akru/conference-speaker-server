#include "receiver.h"

#include <QHostAddress>
#include <QAudioOutput>
#include <QTcpSocket>

Receiver::Receiver(QHostAddress &address, QObject *parent)
    : QObject(parent)
{
    server.setMaxPendingConnections(1);
    if (server.listen(address))
    {
        channel = ChannelInformation(server.serverAddress().toString(),
                                     server.serverPort());
        qDebug() << "Init channel port:" << server.serverPort();
        connect(&server, SIGNAL(newConnection()), SLOT(newConnection()));
        qDebug() << "Listening:" << server.isListening();
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

    // Open buffer
    buffer.open(QIODevice::ReadWrite);
    qDebug() << "opened buffer, size:" << buffer.size();

    audio = new QAudioOutput(info, format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)),
            SLOT(audioStateChanged(QAudio::State)));
    audio->start();
}

void Receiver::audioStateChanged(QAudio::State state)
{
    qDebug() << "New audio state:" << state;
}

void Receiver::newConnection()
{
    client = server.nextPendingConnection();
    qDebug() << "New connection from" << client->peerAddress();
    client->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(client, SIGNAL(readyRead()), SLOT(sockReadyRead()));
}

void Receiver::sockReadyRead()
{
    quint64 written = buffer.write(client->readAll());
    qDebug() << "New data size:" << written << ", buffer size" << buffer.size();

    if (audio->state() == QAudio::IdleState)
        audio->start(&buffer);
}

void Receiver::setVolume(qreal volume)
{
    qDebug() << "Set volume:" << volume;
    audio->setVolume(volume);
}

void Receiver::setAudioDevice(QAudioDeviceInfo &device)
{
    qDebug() << "Set audio device:" << device.deviceName();
    delete audio;
    audio = new QAudioOutput(device, format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)),
            SLOT(audioStateChanged(QAudio::State)));
}
