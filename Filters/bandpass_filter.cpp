#include "bandpass_filter.h"
#include "vector.h"
#include "fir.h"

BandpassFilter::BandpassFilter()
    : fir(new splab::FIR("bandpass", "Bartlett"))
{
    // Design FIR
    fir->setParams(8000, 400, -36, 500, 800, -1, 900, -36);
    fir->design();
    fir->dispInfo();
    // Get params
    splab::Vector<double> coefs = fir->getCoefs();
    foreach (double param, coefs) {
        params.append(param);
    }
    xv.fill(0, params.size());
}

BandpassFilter::~BandpassFilter()
{
    delete fir;
}

QByteArray BandpassFilter::process(QByteArray &data)
{
    QByteArray res;
    res.resize(data.size());

    qint16 *res_p = (qint16 *) res.data();
    qint16 *data_p = (qint16 *) data.data();
    while (res_p != (qint16 *) res.data() + res.size() / 2)
        *res_p++ = firProcess(*data_p++);
    return res;
}

qint16 BandpassFilter::firProcess(qint16 wave)
{
    // SV rift
    xv.pop_front(); xv.push_back(wave);
    // Calc res by params and input
    double result = 0;
    for (int k = 0; k < xv.size(); k++)
        result += xv[k]*params[k];
    return result;
}
