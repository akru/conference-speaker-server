#ifndef BANDSWITCH_FILTER_H
#define BANDSWITCH_FILTER_H

#include "../Speaker/filter.h"
#include "biquad.h"
#include <QVector>
#include <QTimer>

static const int BAND_COUNT   = 2;
static const int FILTER_COUNT = 10;

static const int BAND_SHIFT_TIME = 800; // ms; WARN: it divided by frame length (32 ms)

class BandswitchFilter : public Filter
{
public:
    BandswitchFilter();
    ~BandswitchFilter();

    QByteArray process(const QByteArray &sample);
    QString name() { return "BandSwitch"; }

private:
    QVector<QVector<double> > xv;
    int currentBand, iteration;
    double firProcess(int i, double wave);
};

#endif // BANDSWITCH_FILTER_H
