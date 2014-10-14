#include "compressor_filter.h"
#include <cmath>

#include <QDebug>
#include <QSettings>

#define INT16_DIV (1<<15)

CompressorFilter::CompressorFilter()
    : mThreshold(0.25),
      mNoiseFloor(0.01),
      mCircle(0),
      mFollow1(new float[sample_length]),
      mFollow2(new float[sample_length]),
      mFollowLen(sample_length),
      mSampleIn(new float[sample_length]),
      mSampleOut(new float[sample_length]),
      first_start(true)
{
    NewTrackPass1();
    reloadSettings();
}

CompressorFilter::~CompressorFilter()
{
    delete [] mCircle;
    delete [] mFollow1;
    delete [] mFollow2;
    delete [] mSampleIn;
    delete [] mSampleOut;
}

void CompressorFilter::setParams(bool normalize,
                                 bool use_peak,
                                 float attack_time,
                                 float decay_time,
                                 float ratio,
                                 float compression,
                                 float threshold_db,
                                 float noise_floor_db)
{
    mNormalize = normalize;
    mUsePeak = use_peak;
    mAttackTime = attack_time; // seconds
    mDecayTime = decay_time;   // seconds
    mRatio = ratio;            // positive number > 1.0
    mCompression = compression;
    mThresholdDB = threshold_db;
    mNoiseFloorDB = noise_floor_db;
}

void CompressorFilter::reloadSettings()
{
    QSettings s(settingsFiltename(), QSettings::IniFormat);
    enable(s.value("compressor-enable", true).toBool());
    setParams(s.value("compressor-normalize",true).toBool(),
              s.value("compressor-peak", false).toBool(),
              s.value("compressor-attack", 0.2).toFloat(),
              s.value("compressor-decay", 1.0).toFloat(),
              s.value("compressor-ratio", 2).toFloat(),
              s.value("compressor-compression", 0.5).toFloat(),
              s.value("compressor-threshold", -12).toFloat(),
              s.value("compressor-noise", -40).toFloat());
}

bool CompressorFilter::NewTrackPass1()
{
   mThreshold = pow(10.0, mThresholdDB/20); // factor of 20 because it's power
   mNoiseFloor = pow(10.0, mNoiseFloorDB/20);
   mNoiseCounter = 100;

   mAttackInverseFactor = exp(log(mThreshold) / (sample_rate * mAttackTime + 0.5));
   mAttackFactor = 1.0 / mAttackInverseFactor;
   mDecayFactor = exp(log(mThreshold) / (sample_rate * mDecayTime + 0.5));

   if(mRatio > 1)
      mCompression = 1.0-1.0/mRatio;
   else
      mCompression = 0.0;

   mLastLevel = mThreshold;

   mCircleSize = 100;
   mCircle = new double[mCircleSize];
   memset(mCircle, 0, mCircleSize * sizeof(double));
   mCirclePos = 0;
   mRMSSum = 0.0;

   return true;
}


// Process the input with 2 buffers available at a time
// buffer1 will be written upon return
// buffer2 will be passed as buffer1 on the next call
bool CompressorFilter::TwoBufferProcessPass1(float *buffer1, float *buffer2)
{
   int i;

   // This makes sure that the initial value is well-chosen
   // buffer1 == NULL on the first and only the first call
   if (buffer1 == NULL) {
      // Initialize the mLastLevel to the peak level in the first buffer
      // This avoids problems with large spike events near the beginning of the track
      mLastLevel = mThreshold;
      for(i=0; i<sample_length; i++) {
         if(mLastLevel < fabs(buffer2[i]))
            mLastLevel = fabs(buffer2[i]);
      }
   }

   // buffer2 is NULL on the last and only the last call
   if(buffer2 != NULL) {
      Follow(buffer2, mFollow2, mFollow1);
   }

   if(buffer1 != NULL) {
      for (i = 0; i < sample_length; i++) {
         buffer1[i] = DoCompression(buffer1[i], mFollow1[i]);
      }
      if (mNormalize)
          ProcessPass2(buffer1);
      // Upto
      buffer1[i] *= INT16_DIV;
   }

   // Rotate the buffer pointers
   float *tmpfloat = mFollow1;
   mFollow1 = mFollow2;
   mFollow2 = tmpfloat;

   return true;
}

bool CompressorFilter::ProcessPass2(float *buffer)
{
   if (mMax != 0)
   {
      for (int i = 0; i < sample_length; i++)
         buffer[i] /= mMax;
   }

   return true;
}

void CompressorFilter::FreshenCircle()
{
   // Recompute the RMS sum periodically to prevent accumulation of rounding errors
   // during long waveforms
   mRMSSum = 0;
   for(int i=0; i<mCircleSize; i++)
      mRMSSum += mCircle[i];
}

float CompressorFilter::AvgCircle(float value)
{
   float level;

   // Calculate current level from root-mean-squared of
   // circular buffer ("RMS")
   mRMSSum -= mCircle[mCirclePos];
   mCircle[mCirclePos] = value*value;
   mRMSSum += mCircle[mCirclePos];
   level = sqrt(mRMSSum/mCircleSize);
   mCirclePos = (mCirclePos+1)%mCircleSize;

   return level;
}

void CompressorFilter::Follow(float *buffer, float *env, float *previous)
{
   /*

   "Follow"ing algorithm by Roger B. Dannenberg, taken from
   Nyquist.  His description follows.  -DMM

   Description: this is a sophisticated envelope follower.
    The input is an envelope, e.g. something produced with
    the AVG function. The purpose of this function is to
    generate a smooth envelope that is generally not less
    than the input signal. In other words, we want to "ride"
    the peaks of the signal with a smooth function. The
    algorithm is as follows: keep a current output value
    (called the "value"). The value is allowed to increase
    by at most rise_factor and decrease by at most fall_factor.
    Therefore, the next value should be between
    value * rise_factor and value * fall_factor. If the input
    is in this range, then the next value is simply the input.
    If the input is less than value * fall_factor, then the
    next value is just value * fall_factor, which will be greater
    than the input signal. If the input is greater than value *
    rise_factor, then we compute a rising envelope that meets
    the input value by working bacwards in time, changing the
    previous values to input / rise_factor, input / rise_factor^2,
    input / rise_factor^3, etc. until this new envelope intersects
    the previously computed values. There is only a limited buffer
    in which we can work backwards, so if the new envelope does not
    intersect the old one, then make yet another pass, this time
    from the oldest buffered value forward, increasing on each
    sample by rise_factor to produce a maximal envelope. This will
    still be less than the input.

    The value has a lower limit of floor to make sure value has a
    reasonable positive value from which to begin an attack.
   */
   int i;
   double level,last;

   if(!mUsePeak) {
      // Update RMS sum directly from the circle buffer
      // to avoid accumulation of rounding errors
      FreshenCircle();
   }
   // First apply a peak detect with the requested decay rate
   last = mLastLevel;
   for(i=0; i<sample_length; i++) {
      if(mUsePeak)
         level = fabs(buffer[i]);
      else // use RMS
         level = AvgCircle(buffer[i]);
      // Don't increase gain when signal is continuously below the noise floor
      if(level < mNoiseFloor) {
         mNoiseCounter++;
      } else {
         mNoiseCounter = 0;
      }
      if(mNoiseCounter < 100) {
         last *= mDecayFactor;
         if(last < mThreshold)
            last = mThreshold;
         if(level > last)
            last = level;
      }
      env[i] = last;
   }
   mLastLevel = last;

   // Next do the same process in reverse direction to get the requested attack rate
   last = mLastLevel;
   for(i=sample_length-1; i>=0; i--) {
      last *= mAttackInverseFactor;
      if(last < mThreshold)
         last = mThreshold;
      if(env[i] < last)
         env[i] = last;
      else
         last = env[i];
   }

   if(previous != NULL) {
      // If the previous envelope was passed, propagate the rise back until we intersect
      for(i=sample_length-1; i>0; i--) {
         last *= mAttackInverseFactor;
         if(last < mThreshold)
            last = mThreshold;
         if(previous[i] < last)
            previous[i] = last;
         else // Intersected the previous envelope buffer, so we are finished
            return;
      }
      // If we can't back up far enough, project the starting level forward
      // until we intersect the desired envelope
      last = previous[0];
      for(i=1; i<sample_length; i++) {
         last *= mAttackFactor;
         if(previous[i] > last)
            previous[i] = last;
         else // Intersected the desired envelope, so we are finished
            return;
      }
      // If we still didn't intersect, then continue ramp up into current buffer
      for(i=0; i<sample_length; i++) {
         last *= mAttackFactor;
         if(buffer[i] > last)
            buffer[i] = last;
         else // Finally got an intersect
            return;
      }
      // If we still didn't intersect, then reset mLastLevel
      mLastLevel = last;
   }
}

float CompressorFilter::DoCompression(float value, double env)
{
   float out;
   if(mUsePeak) {
      // Peak values map 1.0 to 1.0 - 'upward' compression
      out = value * pow(1.0/env, mCompression);
   } else {
      // With RMS-based compression don't change values below mThreshold - 'downward' compression
      out = value * pow(mThreshold/env, mCompression);
   }

   // Retain the maximum value for use in the normalization pass
   if(mMax < fabs(out))
      mMax = fabs(out);

//   qDebug() << "val:" << value << "cout:" << out << "max" << mMax;

   return out;
}

void CompressorFilter::processFilter(float sample[])
{
    // Swap buffers
    float *tmp = mSampleIn;
    mSampleIn = mSampleOut;
    mSampleOut = tmp;

    // Read sample
    for (short i = 0; i < sample_length; ++i)
        mSampleIn[i] = sample[i];

    // Process sample
    if (first_start)
    {
        TwoBufferProcessPass1(0, mSampleIn);
        first_start = false;
        memset(mSampleOut, 0, sizeof(float) * sample_length);
    }
    else
        TwoBufferProcessPass1(mSampleOut, mSampleIn);

    // Return sample
    for (short i = 0; i < sample_length; ++i)
        sample[i] = mSampleOut[i];
}
