#include "receiver.h"

#include <QHostAddress>
#include <QAudioOutput>
#include <QTcpSocket>

Receiver::Receiver(QHostAddress address, QObject *parent)
    : QObject(parent),
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

    if (sock.bind(address))
    {
        channel = ChannelInformation(sock.localAddress().toString(),
                                     sock.localPort());
        qDebug() << "Init channel port:" << sock.localPort();

        emit connected(this);
        // Start playing channel
        audio->start(&sock);
    }
    else
        throw(std::exception());
}

void Receiver::audioStateChanged(QAudio::State state)
{
    qDebug() << "New audio state:" << state;
}

void Receiver::setVolume(qreal volume)
{
    qDebug() << "Set volume:" << volume;
    Q_ASSERT(audio);
    audio->setVolume(volume);
}
