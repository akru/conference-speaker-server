#ifndef AGC_FILTER_H
#define AGC_FILTER_H

#include "filter.h"
#include "QTime"

#define AGC_TUNING_TIME 3000 // in ms

class AGCFilter : public Filter
{
public:
    AGCFilter();
    ~AGCFilter();

    void processFilter(float sample[]);
    QString name() { return "AGC"; }
    void reloadSettings() {}

private:
    float maxAmp;
    bool   firstSample;
    void   analyzeMax(float sample[]);
    void   gainSample(float sample[]);
    QTime speakingTime;
};

#endif // AGC_FILTER_H
