#ifndef PITCH_SHIFT_FILTER_H
#define PITCH_SHIFT_FILTER_H

#include "filter.h"

static const int PITCH_COUNT   = 2;
static const int PITCH_SHIFT_TIME = 1200; // ms; WARN: it divided by frame length (32 ms)

typedef struct soxr * soxr_t;

class PitchShiftFilter : public Filter
{
public:
    PitchShiftFilter();
    ~PitchShiftFilter();

    void processFilter(float sample[]);
    QString name() { return "Pitch shift"; }
    void reloadSettings();

private:
    static const short len_scaler = 2;
    static const short analyze_length = sample_length * len_scaler;

    float gInFIFO[analyze_length];
    float gOutFIFO[analyze_length];
    float gFFTworksp[2*analyze_length];
    float gLastPhase[analyze_length/2+1];
    float gSumPhase[analyze_length/2+1];
    float gOutputAccum[2*analyze_length];
    float gAnaFreq[analyze_length];
    float gAnaMagn[analyze_length];
    float gSynFreq[analyze_length];
    float gSynMagn[analyze_length];
    long gRover, osamp;
    float pitchShift, pitchShiftCoef;
    int currentPitch, iteration;

    // Improved resampler using SoX
    soxr_t widener;
    soxr_t zipper;

    void smbPitchShift(long numSampsToProcess,
                       long fftFrameSize,
                       float *indata,
                       float *outdata);
};

#endif // PITCH_SHIFT_FILTER_H
