#include "ns_filter.h"
#include "noise_suppression.h"

#include <cmath>
#include <QDebug>

NSFilter::NSFilter(Level level, int count, int trashold)
    : count(count),
      trashold(trashold)
{
    Q_ASSERT(sample_rate == 8000.0);

    qDebug() << "NSFilter(" << level << ")";

    if (!WebRtcNs_Create(&ns_ptr))
    {
        WebRtcNs_Init(ns_ptr, sample_rate);
        WebRtcNs_set_policy(ns_ptr, level);
    }
    else
        qWarning() << "Unable to create noise supression filter";
}

NSFilter::~NSFilter()
{
    WebRtcNs_Free(ns_ptr);
}

void NSFilter::process(float sample[])
{
//    QByteArray out(sample.length(), Qt::Uninitialized);
//    WebRtcNs_Process(ns_ptr, (short *)sample.data(), NULL,
//                             (short *)out.data(), NULL);

//    postSuppression(sample);
}

void NSFilter::postSuppression(float sample[])
{
    long long energy; // RMS
    for (short pos = 0; pos < sample_length; ++pos)
    {
        energy = 0;
        for (short i = 0; i < count && pos + i < sample_length; ++i)
        {
            qDebug() << "Sample " << pos + i << ": " << sample[pos + i] << ";";
            energy += fabs(sample[pos + i]) * fabs(sample[pos + i]);
        }
        energy = sqrt(energy / count);
        qDebug() << "RMS Energy TH:" << energy;

        if (energy < trashold)
            for (short i = 0; i < count && pos + i < sample_length; ++i)
                sample[pos + i] = 0;
        pos += count;
    }
}
