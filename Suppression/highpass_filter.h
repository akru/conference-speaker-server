#ifndef HIGHTPASS_FILTER_H
#define HIGHTPASS_FILTER_H

#include "../Speaker/filter.h"
#include <QVector>

class HighPassFilter : public Filter
{
public:
    HighPassFilter();
    ~HighPassFilter();

    void process(float sample[]);
    QString name() { return "High pass"; }

private:
    QVector<float> xv;
    float firProcess(float wave);
};

#endif // HIGHTPASS_FILTER_H
