#include "speaker.h"
#include <QAudioOutput>
#include <QAudioFormat>
#include <suppressor.h>

#include <QDebug>

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

Speaker::Speaker(QObject *parent) :
    QObject(parent),
    disabled(false),
    format(new QAudioFormat),
    audio(0)
{
    // Set up the format, eg.
    format->setSampleSize(16);
    format->setChannelCount(1);
#ifdef MACOSX
    format->setSampleRate(48000);
#else
    format->setSampleRate(8000);
#endif
    format->setCodec("audio/pcm");
    format->setSampleType(QAudioFormat::SignedInt);
    format->setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo info =
            QAudioDeviceInfo::defaultOutputDevice();

    if (!info.isFormatSupported(*format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        disabled = true;
        return;
    }

    // Open audio device
    audio = new QAudioOutput(info, *format);
    audio_buffer = audio->start();
    // Append filters
    filters.append(new Suppressor);
}

Speaker::~Speaker()
{
    delete format;
    delete audio;
    foreach (Filter *f, filters) {
        delete f;
    }
}


void Speaker::setVolume(qreal volume)
{
    qDebug() << "Set volume:" << volume;
    Q_ASSERT(audio);
    Q_ASSERT(volume >= 0 && volume <= 1);

    audio->setVolume(volume);
}

void Speaker::play(QByteArray sample)
{
    Q_ASSERT(audio_buffer);
    // Apply filters
    foreach (Filter *f, filters) {
        qDebug() << "Applying:" << f->name();
        sample = f->process(sample);
    }
    // Analyze sample amplitude
    ampAnalyze(sample);
    // Play buffer
#ifdef MACOSX
    data = convertAudio(data);
#endif
    audio_buffer->write(sample);
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
void Speaker::ampAnalyze(QByteArray &sample)
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
