#include "receiver.h"

#include <QHostAddress>
#include <suppressor.h>
#include <lms_filter.h>

#ifdef MACOSX
QByteArray convertAudio(QByteArray &buffer)
{
    QByteArray output;
    QDataStream ds(&output, QIODevice::WriteOnly);
    qint8 *dataPointer = (qint8 *) buffer.data();
    while (dataPointer < (qint8 *) buffer.data() + buffer.size())
    {
        // One wave is 8kHz
        qint8 wave1 = *dataPointer++;
        qint8 wave2 = *dataPointer++;
        // x6 waves give 48kHz
        ds << wave1 << wave2;
        ds << wave1 << wave2;
        ds << wave1 << wave2;
        ds << wave1 << wave2;
        ds << wave1 << wave2;
        ds << wave1 << wave2;
    }
    return output;
}
#endif

Receiver::Receiver(QHostAddress address, QObject *parent)
    : QObject(parent),
      audio(0)
{
    // Set up the format, eg.
    format.setSampleSize(16);
    format.setChannelCount(1);
#ifdef MACOSX
    format.setSampleRate(48000);
#else
    format.setSampleRate(8000);
#endif
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    qDebug() << "Audio output:" << info.deviceName();

    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        throw(std::exception());
    }

    // Open audio device
    audio = new QAudioOutput(info, format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)),
            SLOT(audioStateChanged(QAudio::State)));

    if (sock.bind(address))
    {
        channel = ChannelInformation(sock.localAddress().toString(),
                                     sock.localPort());
        qDebug() << "Init channel port:" << sock.localPort();
        connect(&sock, SIGNAL(readyRead()), SLOT(sockReadyRead()));
        // Start playing channel
        buffer = audio->start();
    }
    else
        throw(std::exception());

    // Setup filters
    filters.append(new Suppressor);
    filters.append(new LMSFilter);

    connect(this,     SIGNAL(sampleReceived(QByteArray)),
            &filters, SLOT(process(QByteArray)));
    connect(&filters, SIGNAL(ready(QByteArray)), SLOT(play(QByteArray)));
}

void Receiver::sockReadyRead()
{
    QByteArray buf;
    // Receive audio sample
    qDebug() << "New data size" << sock.pendingDatagramSize();
    buf.resize(sock.pendingDatagramSize());
    sock.readDatagram(buf.data(), buf.size());
    // Filtering & echo cancellation
    emit sampleReceived(buf);
}

void Receiver::play(QByteArray data)
{
    // Analyze sample amplitude
    ampAnalyze(data);
    // Play buffer
#ifdef MACOSX
    data = convertAudio(data);
#endif
    buffer->write(data);
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

/*
 * Average amplitude analyzer.
 * Formula:
 *   A = (a1 + a2 + ... + aN) / N / 10000
 * where A is normalized average amplitude,
 * aN is custom amplutude from sample
 * and N is count of amplitudes.
 *
 * Sample is a array of 16bit signed integers.
 * PCM 16bit sample.
 *
 * This function returns number from 0 to 100.
 * That shows current sample average amplitude.
 */
void Receiver::ampAnalyze(QByteArray &sample)
{
    Q_ASSERT(sample.size());
    // Variable init
    qint16 *dataPointer = (qint16 *) sample.data();
    double avgAmp = 0;
    int count = sample.size() / 2;
    // Sum all amps from sample
    while (dataPointer < (qint16 *) sample.data() + count)
        avgAmp += abs(*dataPointer++);
    // Divize sum by count and normalize it
    avgAmp = avgAmp / count / 10000;
    // Return average amplitude in percents
    emit audioAmpUpdated(avgAmp * 100);
}
