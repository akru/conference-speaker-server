#include "filter.h"
#include <QCoreApplication>

#ifdef QT_DEBUG
    #include <QDebug>
    #include <cmath>
#endif

static const quint16 norm_int16 = 32768;

QString Filter::settingsFiltename()
{
    return QCoreApplication::applicationDirPath() + "/settings.ini";
}

void Filter::enable(bool e)
{
    enabled = e;
}

void Filter::process(float sample[])
{
#ifdef QT_DEBUG
    float rms = 0;
    for (short i = 0; i < sample_length; ++i)
        rms += sample[i] * sample[i];
    rms = sqrt(rms) / sample_length;
    qDebug() << name() << "input RMS: " << rms;
#endif

    if (enabled)
        processFilter(sample);

#ifdef QT_DEBUG
    rms = 0;
    for (short i = 0; i < sample_length; ++i)
        rms += sample[i] * sample[i];
    rms = sqrt(rms) / sample_length;
    qDebug() << name() << "output RMS: " << rms;
#endif
}

void Filter::fromPCM(qint16 pcm[], float sample[])
{
    // Normalization
    for (short i = 0; i < sample_length; ++i)
        sample[i] = ((float)pcm[i]) / norm_int16;
}

void Filter::toPCM(float sample[], qint16 pcm[])
{
    // Back to the RAW
    for (short i = 0; i < sample_length; ++i)
        pcm[i] = fabs(sample[i]) >= 1.0
                  ? sample[i] / sample[i] * (norm_int16 - 10)
                  : sample[i] * norm_int16;
}

void Filter::applyFilters(QList<Filter *> &filters, QByteArray &raw_sample)
{
    float sample[sample_length];
    fromPCM((qint16 *)raw_sample.data(), sample);
    foreach (Filter *f, filters) {
        f->process(sample);
    }
    toPCM(sample, (qint16 *)raw_sample.data());
}

void Filter::reloadSettings(QList<Filter *> &filters)
{
    foreach (Filter *f, filters) {
        f->reloadSettings();
    }
}
