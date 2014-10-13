#include "speaker.h"

#include <QAudioOutput>
#include <QAudioFormat>

#include <ns_filter.h>
#include <hs_filter.h>
#include <pitch_shift_filter.h>
#include <equalizer_filter.h>

#include <QDebug>

/*
 * Dummy resampler: 22050 -> 44100
 */
void convertAudio(float sample[], float output[])
{
    memset(output, 0, Filter::sample_length*2*sizeof(float));
    for (short i = 0; i < Filter::sample_length; ++i)
        output[2*i] = sample[i];
}

Speaker::Speaker(QObject *parent) :
    QObject(parent),
    disabled(false),
    format(new QAudioFormat),
    audio(0)
{   
    // Set up the format, eg.
    format->setSampleSize(16);
    format->setChannelCount(1);
    format->setSampleRate(44100);
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
    filters.append(new NSFilter(NSFilter::High, 10, 500));
    EqualizerFilter *eq = new EqualizerFilter;
    HSFilter *hs = new HSFilter(eq, 15, 44, 35, 0.3);
    filters.append(hs);
    filters.append(eq);
    // Moving to separate thread
//    this->moveToThread(&myThread);
//    myThread.start(QThread::TimeCriticalPriority);
    // Starting heartbeat timer
    connect(&heartbeat, SIGNAL(timeout()), SLOT(speakHeartbeat()));
    heartbeat.setInterval(Filter::sample_length * 999.0 / Filter::sample_rate);
    heartbeat.start();
}

Speaker::~Speaker()
{
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

    if(volume == 0)
        audio->setVolume(volume);
    else{
        audio->setVolume(volume*1.2);
    }
}

void Speaker::play(QByteArray packet)
{
    Q_ASSERT(audio_buffer);
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
    // Analyze sample amplitude
    ampAnalyze(sample);
    // Resampling
    float output[Filter::sample_length * 2];
    convertAudio(sample_raw, output);
    // Back to the INT
    rawp = (qint16 *) sample_raw.data();
    fltp = sample;
    while ((char *) rawp < sample_raw.data() + sample_raw.length())
        *rawp++ = *fltp++ * norm_int16;
    // Play buffer
    audio_buffer->write(output);
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
void Speaker::ampAnalyze(const float sample[])
{
    // Variable init
    float avgAmp = 0;
    int count = Filter::sample_length;
    // Sum all amps from sample
    for (short i = 0; i < Filter::sample_length; ++i)
        avgAmp += fabs(sample[i]);
    // Divize sum by count and normalize it
    avgAmp = avgAmp / count;
    // Return average amplitude in percents
    emit audioAmpUpdated(avgAmp * 100);
}
