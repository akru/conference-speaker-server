#ifndef PITCH_SHIFT_FILTER_H
#define PITCH_SHIFT_FILTER_H

#include "../Speaker/filter.h"

static const int PITCH_COUNT   = 2;
static const int PITCH_SHIFT_TIME = 1200; // ms; WARN: it divided by frame length (32 ms)

class PitchShiftFilter : public Filter
{
public:
    PitchShiftFilter(float pitch_shift=0.0, long osamp=32);
    ~PitchShiftFilter();

    void processFilter(float sample[]);
    QString name() { return "Pitch shift"; }

private:
    float gInFIFO[sample_length];
    float gOutFIFO[sample_length];
    float gFFTworksp[2*sample_length];
    float gLastPhase[sample_length/2+1];
    float gSumPhase[sample_length/2+1];
    float gOutputAccum[2*sample_length];
    float gAnaFreq[sample_length];
    float gAnaMagn[sample_length];
    float gSynFreq[sample_length];
    float gSynMagn[sample_length];
    long gRover, osamp;
    float pitchShift; // should be in [0; 1), 0.04 - for example
    int currentPitch, iteration;

    void smbPitchShift(long numSampsToProcess,
                       long fftFrameSize,
                       float *indata,
                       float *outdata);
};

#endif // PITCH_SHIFT_FILTER_H
