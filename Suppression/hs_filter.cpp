#include "hs_filter.h"
#include "howling_suppression.h"

#include <QDebug>

HSFilter::HSFilter(quint32 sample_rate,
                   float PAPR_TH, float PHPR_TH, float PNPR_TH)
{
    Q_ASSERT(sample_rate == 8000);

    qDebug() << "HSFilter(" << sample_rate << ")";

    if (!Hs_Create(&hs_ptr))
    {
        Hs_Init(hs_ptr, sample_rate, PAPR_TH, PHPR_TH, PNPR_TH);
    }
    else
        qWarning() << "Unable to create howling supression filter";
}

HSFilter::~HSFilter()
{
    Hs_Free(hs_ptr);
}

QByteArray HSFilter::process(const QByteArray &sample)
{
    Q_ASSERT(sample.length() == 512);

    QByteArray out;
    out.resize(sample.length());

    Hs_Process(hs_ptr, (qint16 *) sample.data(), (qint16 *) out.data());
    return out;
}
