#ifndef COMPRESSOR_FILTER_H
#define COMPRESSOR_FILTER_H

#include "../Speaker/filter.h"

class CompressorFilter : public Filter
{
public:
    CompressorFilter(bool  normalize=true,
                     bool  use_peak=true,
                     float threshold=0.25,
                     float attack_time=0.2,
                     float decay_time=1.0,
                     float ratio=2,
                     float compression=0.5,
                     float threshold_db=-12.0,
                     float noise_floor_db=-40,
                     float noise_floor=0.01);
    ~CompressorFilter();


    void processFilter(float sample[]);
    QString name() { return "Compressor"; }

private:
    bool      TwoBufferProcessPass1(float *buffer1, float *buffer2);
    bool      ProcessPass2(float *buffer);

    bool      NewTrackPass1();
    bool      InitPass1();
    bool      InitPass2();

    void      Follow(float *buffer, float *env, float *previous);
    float     DoCompression(float x, double env);

    bool      mNormalize;	//MJS
    bool      mUsePeak;
    double    mThreshold;
    double    mAttackTime;
    double    mDecayTime;
    double    mRatio;
    double    mCompression;
    double    mThresholdDB;
    double    mNoiseFloorDB;
    double    mNoiseFloor;

    void      FreshenCircle();
    float     AvgCircle(float x);
    double    mRMSSum;
    int       mCircleSize;
    int       mCirclePos;
    double   *mCircle;

    double    mAttackFactor;
    double    mAttackInverseFactor;
    double    mDecayFactor;
    int       mNoiseCounter;
    double    mGain;
    double    mLastLevel;

    float    *mFollow1;
    float    *mFollow2;
    int       mFollowLen;

    float    *mSampleIn;
    float    *mSampleOut;

    double    mMax;			//MJS

    bool      first_start;
};

#endif // COMPRESSOR_FILTER_H
