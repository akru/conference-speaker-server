#include "howling_suppression.h"
#include "windows_private.h"
#include "defines.h"
#include "fft4g.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

#define PI_F 3.1415926535897932384626433832795f

float quad(float a) { return a * a; }
// Max value index
int   maxId(float *arr, int len) {
    float  mVal = arr[0];
    int    mId  = 0;
    for (int i = 1; i < len; ++i)
        if (arr[i] > mVal)
        {
            mVal = arr[i];
            mId = i;
        }
    return mId;
}
// Deviation checker returns 1 when deviation above th
float deviationCheck(int val, int *arr, int len)
{
    for (int i = 0; i < len; ++i)
        if (abs(val - arr[i]) > HS_FREQ_DEVIATION)
            return 1;
    return 0;
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

/*
 * Cascade filter applying
 */
float Hs_BiquadCascade(HsBiquadParams *filter, int count, float in)
{
    while (count--)
        in = Hs_BiquadProcess(filter++, in);
    return in;
}

/*
 * Init IIR filter
 */
void Hs_BiquadInit(HsBiquadParams *inst)
{
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
 * Find howling freques and write into instance array
 * returns: count of found freq
 */
int Hs_AnalyzeHowling(HsHandle *inst, const short *input)
{
    float fin[BLOCKL_MAX];
    float tmpBuf[HS_BLOCKL_MAX];

    // Convert input data to float
    for (short i = 0; i < inst->blockLen; ++i)
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

    // Calc PAPR
    float papr[HS_BLOCKL_MAX],
         *papr_p = papr;
    tmpBuf_p     = tmpBuf;
    while (tmpBuf_p != tmpBuf + HS_BLOCKL_MAX)
        *papr_p++ = 10 * log10f(quad(*tmpBuf_p++) / energy);

    // Evaluate howling freq
    int   hFreqCount = 0,
          maxValId;
    float freq;
    for (short i = 0; i < HS_BIQUAD_COUNT; ++i)
    {
        maxValId = maxId(papr, HS_BLOCKL_MAX);
        // TH check
        if (papr[maxValId] > trashold)
        {
            freq = // Some magic freq converter from index
                    inst->blockLen * 100 * maxValId / HS_BLOCKL_MAX / 2;
            // Down maximum
            papr[maxValId] = 0;
            // Skip freq when deviation is small
            if (!hFreqCount || deviationCheck(freq, inst->howlingFreq, hFreqCount))
            {
                inst->howlingFreq[hFreqCount] = freq;
                ++hFreqCount;
            }
        }
        else
            break;
    }
    return hFreqCount;
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
    for (short i = 0; i < HS_BIQUAD_COUNT; ++i)
        Hs_BiquadInit(inst->filter + i);

    return 0;
}

void Hs_Free(HsHandle *inst)
{
    free(inst);
}

void Hs_Process(HsHandle *inst, const short *input, short *output)
{
    int freqCount = Hs_AnalyzeHowling(inst, input);
    if (!freqCount)
    {
        // Howling not found
        memcpy(output, input, inst->blockLen);
        return;
    }

    // Print information
    fprintf(stderr, "Detected %d hoOowling:\n", freqCount);
    for (short i = 0; i < freqCount; ++i)
        fprintf(stderr, "%d. %d Hz\n", i, inst->howlingFreq[i]);

    // Calc IIR filter
    for (short i = 0; i < freqCount; ++i)
        Hs_BiquadCalc(inst->filter + i,
                      inst->howlingFreq[i] / 100.0 / inst->blockLen);

    // Apply IIR filter
    const short *input_p = input;
    while (input_p < input + inst->blockLen)
        *output++ = Hs_BiquadCascade(inst->filter, freqCount, *input_p++);
}
