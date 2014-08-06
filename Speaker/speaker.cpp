#include "speaker.h"

#include <QAudioOutput>
#include <QAudioFormat>

#include <ns_filter.h>
#include <hs_filter.h>
#include <bandswitch_filter.h>
#include <highpass_filter.h>

#ifdef QT_DEBUG
#include <QTextBrowser>
#include <QTimer>
#endif

#include <QDebug>

const int SAMPLE_RATE = 8000;

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
    format->setSampleRate(SAMPLE_RATE);
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
    filters.append(new NSFilter(SAMPLE_RATE, NSFilter::Low));
    filters.append(new HighPassFilter());
    filters.append(new HSFilter(SAMPLE_RATE,  15, 40, 0, 0.5));
    filters.append(new BandswitchFilter(SAMPLE_RATE));

#ifdef QT_DEBUG
    connect(&debug_dialog, SIGNAL(trasholdes(qreal,qreal,qreal,qreal)),
            SLOT(setTrashHolds(qreal,qreal,qreal,qreal)));
    debug_dialog.show();
    QTimer *t = new QTimer;
    t->setInterval(500);
    connect(t, SIGNAL(timeout()), SLOT(showDebug()));
    t->start();
#endif
}

Speaker::~Speaker()
{
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

#ifdef QT_DEBUG
void Speaker::setTrashHolds(qreal PAPR, qreal PHPR, qreal PNPR, qreal IMSD)
{
    hs->PAPR_TH = PAPR;
    hs->PHPR_TH = PHPR;
    hs->PNPR_TH = PNPR;
    hs->IMSD_TH = IMSD;
}

void Speaker::showDebug()
{
    QString groupTempl = "%1. center <b>%2 Hz</b>; gain <b>%3 dB</b>; count <b>%4</b>:<br>",
            groupFreqTempl = "<span style=\"margin-right: 15px\">%1. <b>%2 Hz</b></span><br>",
            groupText = "";
    for (short i = 0; i < hs->filterCount; ++i)
    {
        groupText += groupTempl.arg(i).arg(hs->group[i].center)
                                      .arg(hs->group[i].gain)
                                      .arg(hs->group[i].freqCount);
        for (short j = 0; j < hs->group[i].freqCount; ++j)
            groupText += groupFreqTempl.arg(j).arg(hs->group[i].freq[j]);
    }
    if (hs->filterCount)
    {
        QTextBrowser *groupList  = debug_dialog.textBrowser();
        groupList->setText(groupText);
    }
}
#endif

void Speaker::play(const QByteArray &packet)
{
    Q_ASSERT(audio_buffer);
    accBuf.putData((qint16 *) packet.data(),
                   packet.length() / sizeof(qint16));
    // When accumulator has too low sounds - skips
    if (!accBuf.avail(512)) return;
    // Prepare sample
    QByteArray sample(512, 0);
    accBuf.getData((qint16 *) sample.data(),
                   sample.length() / sizeof(qint16));
    // Apply filters
    foreach (Filter *f, filters) {
        qDebug() << "Applying:" << f->name();
        sample = f->process(sample);
    }
    // Analyze sample amplitude
    ampAnalyze(sample);
    // Play buffer
#ifdef MACOSX
    sample = convertAudio(sample);
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
