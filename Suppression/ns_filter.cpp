#include "ns_filter.h"
#include "noise_suppression.h"

#include <QDebug>

NSFilter::NSFilter(quint32 sample_rate, Level level)
    : len10ms(sample_rate / 100)
{
    Q_ASSERT(sample_rate == 8000 || sample_rate == 16000);

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
    const short num_iter = sample.length() / 2 / len10ms;
    QByteArray out(num_iter * len10ms * 2, 0);
    qDebug() << "num_iter:" << num_iter << "out buf size:" << out.length();

    const qint16 *in_ptr = (const qint16 *) sample.data();
    qint16 *out_ptr = (qint16 *) out.data();
    for (short i = 0; i < num_iter; ++i)
    {
        qDebug() << "NS process:" <<
                WebRtcNs_Process(ns_ptr, in_ptr, NULL, out_ptr, NULL);
        in_ptr  += len10ms;
        out_ptr += len10ms;
    }
    return out;
}
