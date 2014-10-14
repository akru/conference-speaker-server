#include "ns_filter.h"
#include "noise_suppression.h"

#include <QDebug>
#include <QSettings>

NSFilter::NSFilter()
{
    if (!WebRtcNs_Create(&ns_ptr))
    {
        WebRtcNs_Init(ns_ptr, sample_rate);
        reloadSettings();
    }
    else
        qWarning() << "Unable to create noise supression filter";
}

NSFilter::~NSFilter()
{
    WebRtcNs_Free(ns_ptr);
}

void NSFilter::reloadSettings()
{
    QSettings s(settingsFiltename(), QSettings::IniFormat);
    enable(s.value("ns-enable", true).toBool());
    QString level = s.value("ns-level", "low").toString();
    if (level == "low")
        setPolicy(Low);
    else
    {
        if (level == "medium")
            setPolicy(Medium);
        else
            setPolicy(High);
    }
    qDebug() << "NS settings reloaded";
}

void NSFilter::processFilter(float sample[])
{
    float out[sample_length];

    WebRtcNs_Process(ns_ptr, sample, out);
    WebRtcNs_Process(ns_ptr, sample + Filter::sample_length / 2, out + Filter::sample_length / 2);

    for (short i = 0; i < sample_length; ++i)
        sample[i] = out[i];
}
