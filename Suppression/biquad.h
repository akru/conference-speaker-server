#ifndef BIQUAD_H
#define BIQUAD_H
/*
 * Biquad filter header file.
 */

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    short freq[HS_GROUP_FREQ_MAX], freqCount, center;
    float gain;
} HsFreqGroup;

typedef struct
{
    float a0, a1, a2, b1, b2,
          Q, peakGain, freq,
          z1, z2;
} BiquadParams;

/*
 *Evaluate center of group
 */
short Hs_FreqGroupCenter(HsFreqGroup *);

/*
 * Init IIR filter
 */
void BiquadInit(BiquadParams *);

/*
 * Calc notch biquad IIR coefs
 */
void BiquadCalcNotch(BiquadParams *, HsFreqGroup *group);

/*
 * Calc bandpass biquad IIR coefs
 */
void BiquadCalcBandpass(BiquadParams *);

/*
 * Process IIR filter
 */
float BiquadProcess(BiquadParams *, float);

#ifdef __cplusplus
}
#endif

#endif // BIQUAD_H
