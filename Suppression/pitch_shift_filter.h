#ifndef PITCH_SHIFT_FILTER_H
#define PITCH_SHIFT_FILTER_H

#include "filter.h"
#include <QTime>

static const int PITCH_COUNT   = 2;
static const int PITCH_SHIFT_TIME = 800; // ms; WARN: it divided by frame length (32 ms)

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
    QTime shift_time;

    static const short len_scaler = 2; // only powers of 2
    static const short analyze_length = sample_length * len_scaler;
    static const int   analyze_rate   = sample_rate * len_scaler;

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
    int currentPitch;
//    int iteration;

    float wfft[analyze_length * 2 >> 1];
    int   ip[analyze_length * 2 >> 1];

    // Improved resampler using SoX
    soxr_t widener, zipper;

    void smbPitchShift(long numSampsToProcess,
                       long fftFrameSize,
                       float *indata,
                       float *outdata);
};

#endif // PITCH_SHIFT_FILTER_H
