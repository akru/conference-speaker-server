#include "biquad.h"
#include <math.h>

/*
 * Init IIR filter
 */
void Hs_BiquadInit(HsBiquadParams *inst)
{
    inst->Q = 1;
    inst->peakGain = -3.0;
    inst->a0 = 1.0;
    inst->a1 = inst->a2 = inst->b1 = inst->b2 = 0.0;
    inst->z1 = inst->z2 = 0.0;
}

/*
 * Calc biquad IIR coefs
 */
void Hs_BiquadCalc(HsBiquadParams *inst)
{
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
 * Process IIR filter
 */
float Hs_BiquadProcess(HsBiquadParams *inst, float in)
{
    float out = in * inst->a0 + inst->z1;
    inst->z1  = in * inst->a1 + inst->z2 - inst->b1 * out;
    inst->z2  = in * inst->a2 - inst->b2 * out;
    return out;
}
