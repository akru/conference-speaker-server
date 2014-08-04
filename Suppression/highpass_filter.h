#ifndef HIGHTPASS_FILTER_H
#define HIGHTPASS_FILTER_H

#include "../Speaker/filter.h"
#include <QVector>

class HighPassFilter : public Filter
{
public:
    HighPassFilter();
    ~HighPassFilter();

    QByteArray process(const QByteArray &sample);
    QString name() { return "High pass"; }

private:
    QVector<double> xv;
    double firProcess(double wave);
};

#endif // HIGHTPASS_FILTER_H
