#include "howling_suppression.h"
#include "windows_private.h"
#include "defines.h"
#include "fft4g.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

inline float quad(float a) { return a * a; }

inline float Hs_BiquadProcess(HsBiquadParams *inst, float in) {
    float out = in * inst->a0 + inst->z1;
    inst->z1  = in * inst->a1 + inst->z2 - inst->b1 * out;
    inst->z2  = in * inst->a2 - inst->b2 * out;
    return out;
}

inline void Hs_BiquadInit(HsBiquadParams *inst) {
    inst->Q = 1;
    inst->peakGain = -30.0;
    inst->a0 = 1.0;
    inst->a1 = inst->a2 = inst->b1 = inst->b2 = 0.0;
    inst->z1 = inst->z2 = 0.0;
}

/*
 * Calc biquad IIR coefs
 */
void Hs_BiquadCalc(HsBiquadParams *inst, int Fc)
{
    float V    = powf(10, fabsf(inst->peakGain) / 20.0),
          K    = tanf(M_PI * Fc),
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
 * Find howling freques and write into instance array
 * returns: found freq
 */
int Hs_AnalyzeHowling(HsHandle *inst, const short *input)
{
    short i;
    float fin[BLOCKL_MAX];
    float tmpBuf[HS_BLOCKL_MAX];

    // Convert input data to float
    for (i = 0; i < inst->blockLen; ++i)
        fin[i] = (float) input[i];

    // Update buffers
    memcpy(inst->dataBuf, inst->dataBuf + inst->blockLen,
           sizeof(float) * (HS_BLOCKL_MAX - inst->blockLen));     // Shift
    memcpy(inst->dataBuf + HS_BLOCKL_MAX - inst->blockLen, fin,
           sizeof(float) * inst->blockLen);                       // Copy new data

    // Apply window TODO: overlap
    float *tmpBuf_p  = tmpBuf,
          *dataBuf_p = inst->dataBuf;
    const float
          *window_p  = kBlocks480w1024;
    while (tmpBuf_p < tmpBuf + HS_BLOCKL_MAX)
        *tmpBuf_p++ = *dataBuf_p++ * *window_p++;

    // Apply FFT
    rdft(HS_BLOCKL_MAX, 1, tmpBuf, inst->ip, inst->wfft);

    // Calc trashold
    const float trashold = 10 * log10f(quad(HS_BLOCKL_MAX / 150));

    // Calc energy and filter by TH
    float energy = 0;
    tmpBuf_p = tmpBuf;
    while (tmpBuf_p < tmpBuf + HS_BLOCKL_MAX)
        energy += quad(*tmpBuf_p++);
    energy /= HS_BLOCKL_MAX;

    // Find howling freq
    float papr = 0,
          paprMax = 0;
    int foundFreq = 0;
    for (i = 0; i < HS_BLOCKL_MAX; ++i)
    {
        papr = 10 * log10f(quad(tmpBuf[i]) / energy);
        // TH Filter check
        if(papr > trashold && papr > paprMax)
        {
            paprMax = papr;
            foundFreq = // Some magic freq converter from index
                    inst->blockLen * 100 * i / HS_BLOCKL_MAX / 2;
        }
    }
    return foundFreq;
}

int Hs_Create(HsHandle **inst)
{
    *inst = (HsHandle *) malloc(sizeof(HsHandle));
    if (*inst != NULL) {
      return 0;
    } else {
      return -1;
    }
}

int Hs_Init(HsHandle *inst, int fs)
{
    if (fs != 8000)
        return -1;

    inst->blockLen = fs / 100;
    // Initialize fft work arrays.
    inst->ip[0] = 0; // Setting this triggers initialization.
    memset(inst->dataBuf, 0, sizeof(float) * HS_BLOCKL_MAX);
    rdft(HS_BLOCKL_MAX, 1, inst->dataBuf, inst->ip, inst->wfft);
    memset(inst->dataBuf, 0, sizeof(float) * HS_BLOCKL_MAX);

    // Set filter params
    HsBiquadParams *f = inst->filter;
    Hs_BiquadInit(f);

    return 0;
}

void Hs_Free(HsHandle *inst)
{
    free(inst);
}

void Hs_Process(HsHandle *inst, const short *input, short *output)
{
    int freq = Hs_AnalyzeHowling(inst, input);
    if (!freq)
    {
        // Howling not found
        memcpy(output, input, inst->blockLen);
        return;
    }

    fprintf(stderr, "Detected hoOowling on %d Hz\n", freq);

    // Calc IIR filter
    Hs_BiquadCalc(inst->filter, freq / 100.0 / inst->blockLen);

    // Apply IIR filter
    const short *input_p = input;
    while (input_p < input + inst->blockLen)
        *output++ = Hs_BiquadProcess(inst->filter, *input_p++);
}
