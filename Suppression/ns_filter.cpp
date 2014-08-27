#include "ns_filter.h"
#include "noise_suppression.h"

#include <cmath>
#include <QDebug>

NSFilter::NSFilter(Level level, int count, int trashold)
    : count(count),
      trashold(trashold)
{
    Q_ASSERT(sample_rate == 8000);

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

QByteArray NSFilter::process(const QByteArray &sample)
{
    Q_ASSERT(sample.length() == sample_length * sizeof(qint16));

    QByteArray out(sample.length(), Qt::Uninitialized);
    WebRtcNs_Process(ns_ptr, (short *)sample.data(), NULL,
                             (short *)out.data(), NULL);

    return postSuppression(out);
}

QByteArray NSFilter::postSuppression(const QByteArray &sample)
{
    qint16 *samp = (qint16 *) sample.data();
    long long energy; // RMS
    for (short pos = 0; pos < sample_length; ++pos)
    {
        energy = 0;
        for (short i = 0; i < count && pos + i < sample_length; ++i)
        {
            qDebug() << "Sample " << pos + i << ": " << samp[pos + i] << ";";
            energy += abs(samp[pos + i]) * abs(samp[pos + i]);
        }
        energy = sqrt(energy / count);
        qDebug() << "RMS Energy TH:" << energy;

        if (energy < trashold)
            for (short i = 0; i < count && pos + i < sample_length; ++i)
                samp[pos + i] = 0;
        pos += count;
    }

    return sample;
}
