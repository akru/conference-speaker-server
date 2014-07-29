#include "biquad.h"
#include <math.h>

/*
 * FreqGroup center evaluation
 */
short Hs_FreqGroupCenter(HsFreqGroup *inst)
{
    short sum = 0, *freq = inst->freq;
    while (freq < inst->freq + inst->freqCount)
        sum += *freq++;
    return sum / inst->freqCount;
}

float Hs_FreqGroupQ(HsFreqGroup *inst)
{
    return 60;
}

/*
 * Init IIR filter
 */
void BiquadInit(BiquadParams *inst)
{
    inst->Q = 1;
    inst->peakGain = -3.0;
    inst->a0 = 1.0;
    inst->a1 = inst->a2 = inst->b1 = inst->b2 = 0.0;
    inst->z1 = inst->z2 = 0.0;
}

/*
 * Calc notch biquad IIR coefs
 */
void BiquadCalcNotch(BiquadParams *inst, HsFreqGroup *group)
{
    if (group)
    {
        inst->Q = Hs_FreqGroupQ(group);
        inst->freq = group->center;
        inst->peakGain = group->gain;
    }

    float V    = powf(10, fabsf(inst->peakGain) / 20.0),
          Fc   = inst->freq / 8000.0,
          K    = tanf(PI_F * Fc),
          norm;
    // peak
    if (inst->peakGain >= 0) {    // boost
        norm = 1 / (1 + 1/inst->Q * K + K * K);
        inst->a0 = (1 + V/inst->Q * K + K * K) * norm;
        inst->a1 = 2 * (K * K - 1) * norm;
        inst->a2 = (1 - V/inst->Q * K + K * K) * norm;
        inst->b1 = inst->a1;
        inst->b2 = (1 - 1/inst->Q * K + K * K) * norm;
    }
    else {    // cut
        norm = 1 / (1 + V/inst->Q * K + K * K);
        inst->a0 = (1 + 1/inst->Q * K + K * K) * norm;
        inst->a1 = 2 * (K * K - 1) * norm;
        inst->a2 = (1 - 1/inst->Q * K + K * K) * norm;
        inst->b1 = inst->a1;
        inst->b2 = (1 - V/inst->Q * K + K * K) * norm;
    }
}

/*
 * Calc bandpass biquad IIR coefs
 */
void BiquadCalcBandpass(BiquadParams *inst)
{
    float Fc   = inst->freq / 8000.0,
          K    = tanf(PI_F * Fc),
          norm = 1 / (1 + K / inst->Q + K * K);
    inst->a0 = K / inst->Q * norm;
    inst->a1 = 0;
    inst->a2 = -inst->a0;
    inst->b1 = 2 * (K * K - 1) * norm;
    inst->b2 = (1 - K / inst->Q + K * K) * norm;
}

/*
 * Process IIR filter
 */
float BiquadProcess(BiquadParams *inst, float in)
{
    float out = in * inst->a0 + inst->z1;
    inst->z1  = in * inst->a1 + inst->z2 - inst->b1 * out;
    inst->z2  = in * inst->a2 - inst->b2 * out;
    return out;
}
