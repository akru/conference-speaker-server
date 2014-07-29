#ifndef BANDSWITCH_FILTER_H
#define BANDSWITCH_FILTER_H

#include "../Speaker/filter.h"
#include "biquad.h"
#include <QVector>
#include <QTimer>

static const int BAND_COUNT   = 2;
static const int FILTER_COUNT = 9;
static const int BAND_FREQ[BAND_COUNT][FILTER_COUNT] = {
    {50, 250, 450, 700, 1000, 1370, 1850, 2500, 3400},
    {150, 350, 570, 840, 1170, 1600, 2150, 2900, 4000}
};
static const int BAND_SHIFT_TIME = 10; // ms

class BandswitchFilter : public Filter
{
public:
    BandswitchFilter(quint32 sample_rate);
    ~BandswitchFilter();

    QByteArray process(const QByteArray &sample);
    QString name() { return "BandSwitch"; }

private:
    QVector<QVector<BiquadParams> > filter;
    int currentBand, iteration;
};

#endif // BANDSWITCH_FILTER_H
