#include "speaker.h"
#include "processing.h"
#include "../Suppression/filter.h"

#include <QFile>
#include <QAudioOutput>
#include <QAudioFormat>
#include <exception>
#include <soxr.h>

#include <QDebug>


Speaker::Speaker(QObject *parent) :
    QObject(parent),
    audio(0),
    audio_buffer(0)
{   
    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleSize(16);
    format.setChannelCount(1);
    format.setSampleRate(formatSampleRate);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo info =
            QAudioDeviceInfo::defaultOutputDevice();

    if (!info.isFormatSupported(format)) {
        qWarning() << "supported rates:" << info.supportedSampleRates();
        qWarning() << "supported sizes:" << info.supportedSampleSizes();
        qWarning() << "supported order:" << info.supportedByteOrders();
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        throw std::exception("Raw audio format not supported by backend, cannot play audio.");
    }
    // Create stream resampler
    soxr_error_t error;
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_INT16_I, SOXR_INT16_I);
    soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_HQ, 0);
    resampler = soxr_create(Filter::sample_rate, formatSampleRate,
                            1, &error, &io_spec, &q_spec, NULL);
    if (error) {
        qWarning() << "SoX has an error" << error;
        throw std::exception("SoX has an error!");
    }
    // Open audio device
    audio = new QAudioOutput(info, format);
    // Moving to separate thread
    this->moveToThread(&myThread);
    myThread.start(QThread::TimeCriticalPriority);
    // Starting heartbeat timer
    connect(&heartbeat, SIGNAL(timeout()), SLOT(speakHeartbeat()));
    heartbeat.setInterval(Filter::sample_length * 999.0 / Filter::sample_rate);
    heartbeat.start();
}

Speaker::~Speaker()
{
    audio->stop();
    delete audio;

    foreach (Processing *p, proc.values()) {
        delete p;
    }

    myThread.terminate();
    myThread.wait();
}

void Speaker::speakerNew(QString id)
{
    proc.insert(id, new Processing);
}

void Speaker::speakerDelete(QString id)
{
    delete proc.take(id);
}

void Speaker::setVolume(qreal volume)
{
    qDebug() << "Set global volume:" << volume;
    Q_ASSERT(volume >= 0 && volume <= 1);

    audio->setVolume(volume);
}

void Speaker::setVolume(QString speaker, qreal volume)
{
    qDebug() << "Set speaker " << speaker << "volume:" << volume;
    Q_ASSERT(volume >= 0 && volume <= 1);
}

void Speaker::incomingData(QString speaker, QByteArray packet)
{
    if(!proc.contains(speaker))
    {
        qWarning() << "Unregistered speaker" << speaker;
        return;
    }
    // Put sample to accum
    proc[speaker]->insert(packet);

//    QFile f("/tmp/sample.raw");
//    f.open(QIODevice::Append);
//    f.write(packet);
}

void Speaker::speakHeartbeat()
{
    QByteArray sample, sample_out(Filter::sample_length * 2, 0);
    bool processed = false;
    foreach (Processing *p, proc.values()) {
        sample = p->take();
        // Empty sample when no data to process
        if (!sample.length())
            continue;
        // Processed flag
        processed = true;
        // Emit amplitude signal
        emit audioAmpUpdated(proc.key(p), p->getAmp());
        // Mix samples
        sample_out = Processing::mix(sample, sample_out);
    }
    // Resampling & play
    if (processed) play(sample_out);
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
    foreach (Processing *p, proc.values()) {
        p->reloadSettings();
    }
}
