#include "filter.h"
#include <QCoreApplication>

#include <QDebug>
#include <cmath>

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

