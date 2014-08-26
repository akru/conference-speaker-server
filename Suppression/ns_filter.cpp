#include "ns_filter.h"
#include "noise_suppression.h"

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
    qint16 *sample_p = (qint16 *) sample.data();
    int energy; // RMS
    while ((char *) sample_p < sample.data() + sample.length())
    {
        energy = 0;
        for (short i = 0; i < count && sample_p + i < (qint16 *) sample.data(); ++i)
            energy += sample_p[i] * sample_p[i];
        energy = sqrt(energy/sample.length());

        if (energy < trashold)
            for (short i = 0; i < count && sample_p + i < (qint16 *) sample.data(); ++i)
                *sample_p++ = 0;
        else
            sample_p += count;
    }
    return sample;
}
