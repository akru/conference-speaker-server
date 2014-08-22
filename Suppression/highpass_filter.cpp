#include "highpass_filter.h"

#include <QDebug>
#include "highpass_fir.h"

static const int FIR_LENGTH = sizeof(FIR) / sizeof(double);

HighPassFilter::HighPassFilter()
    : xv(QVector<double>(FIR_LENGTH, 0))
{
    Q_ASSERT(FIR_LENGTH == sample_length);
    qDebug() << "HP filter len" << FIR_LENGTH;
}

HighPassFilter::~HighPassFilter()
{
}

QByteArray HighPassFilter::process(const QByteArray &sample)
{
    Q_ASSERT(sample.length() == sample_length * sizeof(qint16));

    QByteArray out;
    out.resize(sample.length());

    qint16 *inp = (qint16 *) sample.data(),
            *outp = (qint16 *) out.data();
    while ((char *) inp < sample.data() + sample.length())
        *outp++ = firProcess(*inp++);
    return out;
}

double HighPassFilter::firProcess(double wave)
{
    // SV rift
    xv.pop_front(); xv.push_back(wave);
    // Calc res by params and input
    double result = 0;
    for (short k = 0; k < xv.length(); ++k)
        result += xv.at(k) * FIR[k];
    return result;
}
