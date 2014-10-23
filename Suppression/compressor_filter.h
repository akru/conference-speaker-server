#ifndef COMPRESSOR_FILTER_H
#define COMPRESSOR_FILTER_H

#include "filter.h"

class CompressorFilter : public Filter
{
public:
    CompressorFilter();
    ~CompressorFilter();

    void setParams(bool  normalize=true,
                   bool  use_peak=true,
                   float attack_time=0.2,
                   float decay_time=1.0,
                   float ratio=10,
                   float compression=0.5,
                   float threshold_db=-6.0,
                   float noise_floor_db=-40);


    void processFilter(float sample[]);
    QString name() { return "Compressor"; }
    void reloadSettings();

private:
    bool      TwoBufferProcessPass1(float *buffer1, float *buffer2);
    bool      ProcessPass2(float *buffer);

    bool      NewTrackPass1();
    bool      InitPass1();
    bool      InitPass2();

    void      Follow(float *buffer, float *env, float *previous);
    float     DoCompression(float x, float env);

    bool      mNormalize;	//MJS
    bool      mUsePeak;
    float    mThreshold;
    float    mAttackTime;
    float    mDecayTime;
    float    mRatio;
    float    mCompression;
    float    mThresholdDB;
    float    mNoiseFloorDB;
    float    mNoiseFloor;

    void      FreshenCircle();
    float     AvgCircle(float x);
    float    mRMSSum;
    int       mCircleSize;
    int       mCirclePos;
    float   *mCircle;

    float    mAttackFactor;
    float    mAttackInverseFactor;
    float    mDecayFactor;
    int       mNoiseCounter;
    float    mGain;
    float    mLastLevel;

    float    *mFollow1;
    float    *mFollow2;
    int       mFollowLen;

    float    *mSampleIn;
    float    *mSampleOut;

    float    mMax;			//MJS

    bool      first_start;
};

#endif // COMPRESSOR_FILTER_H
