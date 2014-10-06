#include "agc_filter.h"

AGCFilter::AGCFilter()
    : maxAmp(0)
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
    for (short i = 0; i < sample_length; ++i)
        if (sample[i] > maxAmp)
            maxAmp = sample[i];
}

void AGCFilter::gainSample(float sample[])
{
    const float scaler = 1.0 / maxAmp;
    for (short i = 0; i < sample_length; ++i)
        sample[i] *= scaler;
}
