#ifndef BIQUAD_H
#define BIQUAD_H
/*
 * Biquad filter header file.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PI_F 3.1415926535897932384626433832795f

typedef struct
{
    float a0, a1, a2, b1, b2,
          Q, peakGain, freq,
          z1, z2;
} HsBiquadParams;

/*
 * Init IIR filter
 */
void Hs_BiquadInit(HsBiquadParams *);

/*
 * Calc biquad IIR coefs
 */
void Hs_BiquadCalc(HsBiquadParams *);

/*
 * Process IIR filter
 */
float Hs_BiquadProcess(HsBiquadParams *, float);

#ifdef __cplusplus
}
#endif

#endif // BIQUAD_H
