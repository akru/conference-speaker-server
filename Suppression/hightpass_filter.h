#ifndef HIGHTPASS_FILTER_H
#define HIGHTPASS_FILTER_H

#include "../Speaker/filter.h"
#include <QVector>

class HightPassFilter : public Filter
{
public:
    HightPassFilter();
    ~HightPassFilter();

    QByteArray process(const QByteArray &sample);
    QString name() { return "High pass"; }

private:
    QVector<float> xv;
    float firProcess(float wave);
};

#endif // HIGHTPASS_FILTER_H
