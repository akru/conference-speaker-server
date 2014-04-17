#ifndef BANDPASS_FILTER_H
#define BANDPASS_FILTER_H

#include "filter.h"
#include <QVector>

namespace splab {
class FIR;
}

class BandpassFilter : public Filter
{
public:
    BandpassFilter();
    ~BandpassFilter();

    QByteArray process(QByteArray &data);

private:
    qint16 firProcess(qint16 wave);

    splab::FIR *fir;
    QVector<double> params, xv;
};

#endif // BANDPASS_FILTER_H
