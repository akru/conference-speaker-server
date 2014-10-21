#include "speaker.h"

#include <QAudioOutput>
#include <QAudioFormat>

#include <ns_filter.h>
#include <hs_filter.h>
#include <pitch_shift_filter.h>
#include <equalizer_filter.h>
#include <agc_filter.h>
#include <gate_filter.h>
#include <compressor_filter.h>
#include <soxr.h>

//#include <QFile>
#include <QDebug>

Speaker::Speaker(QObject *parent) :
    QObject(parent),
    disabled(false),
    format(new QAudioFormat),
    audio(0)
{   
    // Set up the format, eg.
    format->setSampleSize(16);
    format->setChannelCount(1);
    format->setSampleRate(formatSampleRate);
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
    // Create stream resampler
    soxr_error_t error;
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_INT16_I, SOXR_INT16_I);
    soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_HQ, 0);
    resampler = soxr_create(Filter::sample_rate, formatSampleRate,
                            1, &error, &io_spec, &q_spec, NULL);
    if (error) {
        qWarning() << "SoX has an error" << error;
        disabled = true;
        return;
    }
    // Open audio device
    audio = new QAudioOutput(info, *format);
    audio_buffer = audio->start();
    // Append filters
    amp = new AmpAnalyzeFilter;
    filters.append(amp);
    filters.append(new AGCFilter);
    filters.append(new GateFilter);
    filters.append(new NSFilter);
    filters.append(new CompressorFilter);
    EqualizerFilter *eq = new EqualizerFilter;
    HSFilter *hs = new HSFilter(eq);
    filters.append(hs);
    filters.append(eq);
    filters.append(new PitchShiftFilter);
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

void Speaker::incomingData(QByteArray packet)
{
    // Put sample to accum
    accBuf.putData((qint16 *) packet.data(),
                   packet.length() / sizeof(qint16));

//    QFile f("/tmp/sample.raw");
//    f.open(QIODevice::Append);
//    f.write(packet);
}

void Speaker::speakHeartbeat()
{
    QByteArray sample_raw(Filter::sample_length * sizeof(qint16),
                          Qt::Uninitialized);
    // When accumulator has too low sounds - skips
    if (!accBuf.avail(Filter::sample_length * sizeof(qint16) * 2)) return;
    // Drop too old samples from acc
    accBuf.purity(Filter::sample_length * sizeof(qint16) * 4);
    // Prepare sample
    accBuf.getData((qint16 *) sample_raw.data(), Filter::sample_length);
    // Apply filters
    Filter::applyFilters(filters, sample_raw);
    // Emit amplitude signal
    emit audioAmpUpdated(amp->getAmp());
    // Resampling & play
    play(sample_raw);
}

void Speaker::play(const QByteArray &sample)
{
    QByteArray sample_out(Filter::sample_length * 4, Qt::Uninitialized);
    size_t idone, odone;
    // Resampling to output freq
    soxr_process(resampler,
                 sample.data(),     Filter::sample_length,   &idone,
                 sample_out.data(), Filter::sample_length*2, &odone);
    qDebug() << "Speaker resampler:" << soxr_engine(resampler)
             << "delay" << soxr_delay(resampler);
    // Play buffer
    audio_buffer->write(sample_out);
}

void Speaker::reloadFilterSettings()
{
    Filter::reloadSettings(filters);
}
