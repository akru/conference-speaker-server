#include "filter.h"
#include <QCoreApplication>

#include <QDebug>
#include <cmath>


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
    bool distortion = false;
    for (short i = 0; i < sample_length; ++i) {
        if(sample[i] >= 1.0)
            distortion = true;
        rms += sample[i] * sample[i];
    }
    rms = sqrt(rms) / sample_length;
    qDebug() << name() << "input RMS: " << rms;
        if(distortion)
            qDebug() << name() << "input bigger that one!";
#endif

    if (enabled)
        processFilter(sample);

#ifdef QT_DEBUG
    rms = 0;
    distortion = false;
    for (short i = 0; i < sample_length; ++i){
        if(sample[i] >= 1.0)
            distortion = true;
        rms += sample[i] * sample[i];
    }
    rms = sqrt(rms) / sample_length;
    qDebug() << name() << "output RMS: " << rms;

    for (short i = 0; i < sample_length; ++i)
        if(distortion)
            qDebug() << name() << "output bigger that one!";
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
    bool distortion = false;
    for (short i = 0; i < sample_length; ++i) {
        if(sample[i] > 1.0)
            distortion = true;
        if(distortion)
            qDebug() << "Distortion detected!";
        pcm[i] = fabs(sample[i]) >= 1.0
                  ? sample[i] / sample[i] * (norm_int16 - 10)
                  : sample[i] * norm_int16;
    }
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
