#include "hightpass_filter.h"

#include <QDebug>

static float FIR[] = {
    0.000000, 0.000104, 0.000223, 0.000285, 0.000221,
    0.000000, -0.000329, -0.000638, -0.000758, -0.000551,
    -0.000000, 0.000745, 0.001389, 0.001591, 0.001121,
    0.000000, -0.001438, -0.002620, -0.002940, -0.002033,
    -0.000000, 0.002523, 0.004532, 0.005020, 0.003431,
    -0.000000, -0.004175, -0.007439, -0.008184, -0.005561,
    -0.000000, 0.006715, 0.011943, 0.013136, 0.008938,
    0.000000, -0.010892, -0.019530, -0.021723, -0.015004,
    -0.000000, 0.019121, 0.035425, 0.041120, 0.030050,
    0.000000, -0.046012, -0.099983, -0.150746, -0.186906,
    0.800000, -0.186906, -0.150746, -0.099983, -0.046012,
    0.000000, 0.030050, 0.041120, 0.035425, 0.019121,
    -0.000000, -0.015004, -0.021723, -0.019530, -0.010892,
    0.000000, 0.008938, 0.013136, 0.011943, 0.006715,
    -0.000000, -0.005561, -0.008184, -0.007439, -0.004175,
    -0.000000, 0.003431, 0.005020, 0.004532, 0.002523,
    -0.000000, -0.002033, -0.002940, -0.002620, -0.001438,
    0.000000, 0.001121, 0.001591, 0.001389, 0.000745,
    -0.000000, -0.000551, -0.000758, -0.000638, -0.000329,
    0.000000, 0.000221, 0.000285, 0.000223, 0.000104,
    0.000000
};
static const int FIR_LENGTH = sizeof(FIR) / sizeof(float);

HightPassFilter::HightPassFilter()
    : xv(QVector<float>(FIR_LENGTH, 0))
{
}

HightPassFilter::~HightPassFilter()
{
}

QByteArray HightPassFilter::process(const QByteArray &sample)
{
    QByteArray out;
    out.resize(sample.length());

    qint16 *inp = (qint16 *) sample.data(),
            *outp = (qint16 *) out.data();
    while ((char *) inp < sample.data() + sample.length())
        *outp++ = firProcess(*inp++);
    return out;
}

float HightPassFilter::firProcess(float wave)
{
    // SV rift
    xv.pop_front(); xv.push_back(wave);
    // Calc res by params and input
    double result = 0;
    for (short k = 0; k < xv.length(); ++k)
        result += xv.at(k) * FIR[k];
    return result;
}
