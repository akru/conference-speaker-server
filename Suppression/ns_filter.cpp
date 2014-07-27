#include "ns_filter.h"
#include "noise_suppression.h"

#include <QDebug>

NSFilter::NSFilter(quint32 sample_rate, Level level)
    : len10ms(sample_rate / 100)
{
    Q_ASSERT(sample_rate == 8000);

    qDebug() << "NSFilter(" << sample_rate << "," << level << ")";

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
    Q_ASSERT(sample.length() == 512);
    QByteArray out;
    out.resize(sample.length());
    qDebug() << "NS process:" <<
            WebRtcNs_Process(ns_ptr, (short *)sample.data(), NULL,
                                     (short *)out.data(), NULL);
    return out;
}
