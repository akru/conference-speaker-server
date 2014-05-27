#ifndef LMS_FILTER_H
#define LMS_FILTER_H

#include "filter.h"
#include "vector.h"
#include <QString>

class LMSFilter : public Filter
{
public:
    LMSFilter(uint length=512, double rho = 1.1, double gamma = 0.1);

    QByteArray process(QByteArray &data);
    QString name() { return "LMS"; }

    void dump_stats();

private:
    double rho, gamma;
    splab::Vector<double> wn;
    QByteArray xk, ek;
};

#endif // LMS_FILTER_H
