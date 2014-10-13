#include "agc_filter.h"
#include <cmath>

AGCFilter::AGCFilter()
    : maxAmp(0), firstSample(true)
{
}

AGCFilter::~AGCFilter()
{
}

void AGCFilter::processFilter(float sample[])
{
    analyzeMax(sample);
    gainSample(sample);
}

void AGCFilter::analyzeMax(float sample[])
{
    if (firstSample) { // ignoring first sample
        firstSample = false;
        return;
    }

    for (short i = 0; i < sample_length; ++i) {
        if (fabs(sample[i]) > maxAmp)
            maxAmp = fabs(sample[i]);
    }
}

void AGCFilter::gainSample(float sample[])
{
    if(!maxAmp) return;

    const float scaler = 1.0 / maxAmp;
    for (short i = 0; i < sample_length; ++i)
        sample[i] *= scaler;
}
