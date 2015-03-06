#include "speaker.h"
#include "user.h"
#include "../Suppression/filter.h"

#include <QTime>
#include <QFile>
#include <QAudioOutput>
#include <QAudioFormat>
#include <processing.h>
#include <exception>
#include <soxr.h>

#include <QDebug>

static Speaker *self = 0;

Speaker::Speaker(QObject *parent) :
    QObject(parent),
    disabled(false),
    audio(0),
    audio_buffer(0),
    resampler(0)
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
    qDebug() << "Speaker :: using" << info.deviceName();
    qDebug() << "Speaker ::     where support";
    qDebug() << "Speaker ::         codec:" << info.supportedCodecs();
    qDebug() << "Speaker ::         rates:" << info.supportedSampleRates();
    qDebug() << "Speaker ::         sizes:" << info.supportedSampleSizes();
    qDebug() << "Speaker ::         types:" << info.supportedSampleTypes();
    qDebug() << "Speaker ::         chans:" << info.supportedChannelCounts();
    qDebug() << "Speaker ::         ords:"  << info.supportedByteOrders();

    if (!info.isFormatSupported(format)) {
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
        qWarning() << "SoXr :: has an error" << error;
        disabled = true;
        return;
    }
    qDebug() << "Speaker :: resampler is" << soxr_engine(resampler)
             << "delay" << soxr_delay(resampler);

    // Open audio device
    audio = new QAudioOutput(info, format);
    audio_buffer = audio->start();
    // Connect audio sample processing signals
    connect(this,
            SIGNAL(sampleReady(QByteArray)),
            SLOT(play(QByteArray)));
    // Moving to separate thread
    this->moveToThread(&myThread);
    // Setting heartbeat timer
    heartbeat.setInterval(heartBeatInterval);
    connect(&heartbeat, SIGNAL(timeout()),
                        SLOT(speakHeartbeat()));
    connect(&myThread,  SIGNAL(started()),
            &heartbeat, SLOT(start()));
    // Start the thread
    myThread.start(QThread::TimeCriticalPriority);
}

//Speaker::~Speaker()
//{
//    // Stop audio
//    if (audio)
//    {
//        audio->stop();
//        delete audio;
//    }
//    // Delete processing cycle
//    foreach (Processing *p, proc.values()) {
//        delete p;
//    }
//    // Delete resampler
//    soxr_delete(resampler);
//    // Terminate thread
//    myThread.terminate();
//    myThread.wait();
//}

Speaker * Speaker::instance()
{
    // Singletone constructor
    if (!self)
        self = new Speaker(0);
    return self;
}

void Speaker::speakerNew(User *id)
{
    proc.insert(id, new Processing);
}

void Speaker::speakerDelete(User *id)
{
    if (proc.contains(id))
        delete proc.take(id);
}

void Speaker::setVolume(qreal volume)
{
    qDebug() << "Set global volume:" << volume;
    Q_ASSERT(volume >= 0 && volume <= 1);
    // Disabled behaviour
    if (disabled) return;
    audio->setVolume(volume);
}

void Speaker::setVolume(User *speaker, qreal volume)
{
    qDebug() << "Set speaker" << speaker->getAddress()
             << "volume:" << volume;
    if(!proc.contains(speaker))
    {
        qWarning() << "Unregistered speaker" << speaker->getAddress();
        return;
    }
    // Disabled behaviour
    if (disabled) return;
    // Set local speaker volume
    proc[speaker]->setVolume(volume);
}

void Speaker::incomingData(User *speaker, QByteArray packet)
{
    if(!proc.contains(speaker))
    {
        qWarning() << "Unregistered speaker" << speaker->getAddress();
        return;
    }
    // Disabled behaviour
    if (disabled) return;
    // Put sample to accum
    proc[speaker]->insert(packet);
}

void Speaker::speakHeartbeat()
{
    QByteArray sample, sample_out(Filter::sample_length * 2, 0);
    bool silence = true;
    foreach (User *speaker, proc.keys()) {
        QTime t = QTime::currentTime();
        Processing *p = proc[speaker];
        sample = p->take();
        qDebug() << "Processing elapsed" << t.elapsed() << "ms";
        // Empty sample when no data to process
        if (!sample.length())
            continue;
        else
            silence = false;
        // Emit amplitude signal
        emit audioAmpUpdated(speaker, p->getAmp());
        // Emit unmixed sample
        emit sampleReady(speaker, sample);
        // Mix samples
        sample_out = Processing::mix(sample, sample_out);
    }
    // Emit mixed sample
    if (!silence)
        emit sampleReady(sample_out);
}

void Speaker::play(QByteArray sample)
{
    QByteArray sample_out(Filter::sample_length * 4, Qt::Uninitialized);
    size_t idone, odone;
    // Resampling to output freq
    soxr_process(resampler,
                 sample.data(),     Filter::sample_length,   &idone,
                 sample_out.data(), Filter::sample_length*2, &odone);
    // Play buffer
    audio_buffer->write(sample_out);
}

void Speaker::reloadFilterSettings()
{
    foreach (Processing *p, proc.values()) {
        p->reloadSettings();
    }
}
