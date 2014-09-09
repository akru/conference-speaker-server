#include "highpass_filter.h"

#include <QDebug>
#include "highpass_fir.h"

static const int FIR_LENGTH = sizeof(FIR) / sizeof(double);

HighPassFilter::HighPassFilter()
    : xv(QVector<float>(FIR_LENGTH, 0))
{
    Q_ASSERT(FIR_LENGTH == sample_length);
    qDebug() << "HP filter len" << FIR_LENGTH;
}

HighPassFilter::~HighPassFilter()
{
}

void HighPassFilter::process(float sample[])
{
    for (short i = 0; i < sample_length; ++i)
    {
        *sample = firProcess(*sample);
        ++sample;
    }
}

float HighPassFilter::firProcess(float wave)
{
    // SV rift
    xv.pop_front(); xv.push_back(wave);
    // Calc res by params and input
    float result = 0;
    for (short k = 0; k < sample_length; ++k)
        result += xv[k] * FIR[k];
    return result;
}
