#include "receiver.h"
#include <QHostAddress>
#include <QAudioOutput>

Receiver::Receiver(QObject *parent)
    : QObject(parent)
{
    if (sock.bind())
    {
        channel = ChannelInformation(sock.localPort());
        qDebug() << "Open channel:" << channel.toJson();
        connect(&sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
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

    foreach (const QAudioDeviceInfo &deviceInfo,
             QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        qDebug() << "Device name: " << deviceInfo.deviceName();

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    // Open buffer
    buffer.open(QIODevice::ReadWrite);
    qDebug() << "opened buffer, size:" << buffer.size();

    audio = new QAudioOutput(format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), SLOT(audioStateChanged(QAudio::State)));
}

void Receiver::audioStateChanged(QAudio::State state)
{
    qDebug() << state;
}

void Receiver::sockReadyRead()
{
    quint64 written = buffer.write(sock.readAll());
    qDebug() << "New data in" << channel.port << ", size:" << written;

    if (audio->state() == QAudio::IdleState)
        audio->start(&buffer);
}

void Receiver::setVolume(qreal volume)
{
    audio->setVolume(volume);
}
