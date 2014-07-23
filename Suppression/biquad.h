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
} HsBiquadParams;

/*
 *Evaluate center of group
 */
short Hs_FreqGroupCenter(HsFreqGroup *);

/*
 * Init IIR filter
 */
void Hs_BiquadInit(HsBiquadParams *);

/*
 * Calc biquad IIR coefs
 */
void Hs_BiquadCalc(HsBiquadParams *, HsFreqGroup *group);

/*
 * Process IIR filter
 */
float Hs_BiquadProcess(HsBiquadParams *, float);

#ifdef __cplusplus
}
#endif

#endif // BIQUAD_H
