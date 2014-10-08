#ifndef AGC_FILTER_H
#define AGC_FILTER_H

#include "../Speaker/filter.h"

class AGCFilter : public Filter
{
public:
    AGCFilter();
    ~AGCFilter();

    void processFilter(float sample[]);
    QString name() { return "AGC"; }

private:
    float maxAmp;
    bool  firstSample;
    void  analyzeMax(float sample[]);
    void  gainSample(float sample[]);
    bool firstSample;
};

#endif // AGC_FILTER_H
