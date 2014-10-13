#include "filter.h"
#include <QCoreApplication>

#ifdef QT_DEBUG
    #include <QDebug>
    #include <cmath>
#endif

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
        rms += sample[i]*sample[i];
    rms = sqrt(rms)/sample_length;
    qDebug() << "Input RMS: " << rms;
#endif

    if (enabled)
        processFilter(sample);

#ifdef QT_DEBUG
    rms = 0;
    for (short i = 0; i < sample_length; ++i)
        rms += sample[i]*sample[i];
    rms = sqrt(rms)/sample_length;
    qDebug() << "Output RMS: " << rms;
#endif
}
