#include "hs_filter.h"
#include "howling_suppression.h"

#include <QDebug>

HSFilter::HSFilter(quint32 sample_rate)
{
    Q_ASSERT(sample_rate == 8000);

    qDebug() << "HSFilter(" << sample_rate << ")";

    if (!Hs_Create(&hs_ptr))
    {
        Hs_Init(hs_ptr, sample_rate);
    }
    else
        qWarning() << "Unable to create howling supression filter";
}

HSFilter::~HSFilter()
{
    Hs_Free(hs_ptr);
}

Sample HSFilter::process(const Sample &sample)
{
    const short len10ms = 80;
    const short num_iter = sample.length() / 2 / len10ms;
    QByteArray out(num_iter * len10ms * 2, 0);
    qDebug() << "num_iter:" << num_iter << "out buf size:" << out.length();

    const qint16 *in_ptr = sample.data();
    qint16 *out_ptr = (qint16 *) out.data();
    for (short i = 0; i < num_iter; ++i)
    {
        Hs_Process(hs_ptr, in_ptr, out_ptr);
        in_ptr  += len10ms;
        out_ptr += len10ms;
    }
    return Sample(sample.meta(), out);
}
