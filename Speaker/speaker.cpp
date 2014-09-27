#include "speaker.h"

#include <QAudioOutput>
#include <QAudioFormat>

#include <ns_filter.h>
#include <hs_filter.h>
#include <pitch_shift_filter.h>
#include <equalizer_filter.h>

#include <QDebug>

#ifdef MACOSX
Sample convertAudio(Sample &sample)
{
    QByteArray output;
    QDataStream ds(&output, QIODevice::WriteOnly);
    qint8 *dataPointer = (qint8 *) sample.data();
    while (dataPointer < (qint8 *) sample.data() + sample.length())
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
    return Sample(output);
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
    format->setSampleRate(Filter::sample_rate);
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
#ifndef QT_DEBUG
    // Append filters
    filters.append(new NSFilter(NSFilter::High, 10, 500));
//    filters.append(new HighPassFilter);
    //filters.append(new HSFilter(15, 40, 0, 0.3));
    //filters.append(new BandswitchFilter);
#else

    EqualizerFilter *eq = new EqualizerFilter;
    HSFilter *hs = new HSFilter(eq, 15, 40, 0, 0.3);
    // Append filters
    filters.append(hs);
    filters.append(eq);
    filters.append(new NSFilter(NSFilter::High, 10, 500));

    debug_dialog = new DebugDialog(eq, hs);
    debug_dialog->show();
    QTimer *t = new QTimer;
    t->setInterval(500);
    connect(t, SIGNAL(timeout()), SLOT(showDebug()));
    t->start();
#endif
    // Moving to separate thread
//    this->moveToThread(&myThread);
//    myThread.start(QThread::TimeCriticalPriority);
    // Starting heartbeat timer
    connect(&heartbeat, SIGNAL(timeout()), SLOT(speakHeartbeat()));
    heartbeat.setInterval(Filter::sample_length * 1000.0 / Filter::sample_rate);
    heartbeat.start();
}

Speaker::~Speaker()
{
#ifdef QT_DEBUG
    delete debug_dialog;
#endif
//    myThread.terminate();
//    myThread.wait();

    audio->stop();

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

void Speaker::play(QByteArray packet)
{
    Q_ASSERT(audio_buffer);
    // Analyze sample amplitude
    ampAnalyze(packet);
    // Put sample to accum
    accBuf.putData((qint16 *) packet.data(),
                   packet.length() / sizeof(qint16));
}

void Speaker::speakHeartbeat()
{
    // When accumulator has too low sounds - skips
    if (!accBuf.avail(2048)) return;
    qDebug() << "Playing...";
    // Prepare sample
    QByteArray sample_raw(Filter::sample_length * sizeof(qint16),
                          Qt::Uninitialized);
    accBuf.getData((qint16 *) sample_raw.data(), Filter::sample_length);
    float sample[Filter::sample_length];
    // Normalization
    qint16 *rawp = (qint16 *) sample_raw.data();
    float *fltp = sample;
    while ((char *) rawp < sample_raw.data() + sample_raw.length())
        *fltp++ = ((float) *rawp++) / norm_int16;
    // Apply filters in float area
    foreach (Filter *f, filters) {
        qDebug() << "Applying:" << f->name();
        f->process(sample);
    }
    // Back to the INT
    rawp = (qint16 *) sample_raw.data();
    fltp = sample;
    while ((char *) rawp < sample_raw.data() + sample_raw.length())
        *rawp++ = *fltp++ * norm_int16;
    // Play buffer
#ifdef MACOSX
    sample = convertAudio(sample_raw);
#endif
    audio_buffer->write(sample_raw);
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
void Speaker::ampAnalyze(const QByteArray &sample)
{
    Q_ASSERT(sample.length());
    // Variable init
    const qint16 *dataPointer = (const qint16 *) sample.data();
    double avgAmp = 0;
    int count = sample.length() / 2;
    // Sum all amps from sample
    while ((char *) dataPointer < sample.data() + count)
        avgAmp += abs(*dataPointer++);
    // Divize sum by count and normalize it
    avgAmp = avgAmp / count / 10000;
    // Return average amplitude in percents
    emit audioAmpUpdated(avgAmp * 100);
}
