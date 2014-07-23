#include "howling_suppression.h"
#include "windows_private.h"
#include "defines.h"
#include "fft4g.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

float quad(float a) { return a * a; }
// Deviation checker returns index of element
int deviationCheck(int val, int *arr, int len)
{
    for (int i = 0; i < len; ++i)
        if (abs(val - arr[i]) < HS_FREQ_DEVIATION)
            return i;
    return -1;
}

/*
 * Cascade filter applying
 */
float Hs_BiquadCascade(HsHandle *inst, float in)
{
    int count              = inst->filterCount;
    HsBiquadParams *filter = inst->filter;
    while (count--)
        in = Hs_BiquadProcess(filter++, in);
    return in;
}

/*
 * Filter updater
 */
void Hs_BiquadUpdate(HsHandle *inst, short howlingFreq[], int freqCount)
{
    HsFreqGroup group[HS_BIQUAD_COUNT];
    memset(group, 0, sizeof(HsFreqGroup) * HS_BIQUAD_COUNT);
    short       groupCount = 0;
    // Enumerate howling freq's
    for (short i = 0; i < freqCount && groupCount < HS_BIQUAD_COUNT; ++i)
    {
        for (short j = 0; j  < groupCount; ++j)
            if (abs(howlingFreq[i] - group[j].center) < HS_FREQ_DEVIATION)
            {
                group[j].freq[group[j].freqCount++] = howlingFreq[i];
                group[j].center = Hs_FreqGroupCenter(group + j);
                // Down freq
                howlingFreq[i] = 0;
                break;
            }
        // When freq is not used - create group
        if (howlingFreq[i])
        {
            group[groupCount].freqCount = 1;
            group[groupCount].freq[0]   = howlingFreq[i];
            group[groupCount].gain      = -HS_GAIN_DOWN_STEP;
            group[groupCount].center    = group[groupCount].freq[0];
            ++groupCount;
        }
    }
    // Get exist filters
    for (short i = 0; i < inst->filterCount; ++i)
    {
        char founded = 0;
        for (short j = 0; j < groupCount; ++j)
        {
            if (abs(inst->filter[i].freq - group[j].center) < HS_FREQ_DEVIATION)
            {
                group[j].freq[group[j].freqCount++] = inst->filter[i].freq;
                group[j].center = Hs_FreqGroupCenter(group + j);

                if (inst->filter[i].peakGain > HS_GAIN_MIN)
                    group[j].gain = inst->filter[i].peakGain - HS_GAIN_DOWN_STEP;
                else
                    group[j].gain = inst->filter[i].peakGain;

                founded = 1;
            }
        }
        if (!founded)
        {
            group[groupCount].freqCount = 1;
            group[groupCount].freq[0] = inst->filter[i].freq;
            group[groupCount].center  = group[groupCount].freq[0];
            group[groupCount].gain    = inst->filter[i].peakGain + HS_GAIN_UP_STEP;
            ++groupCount;
        }
    }
    // Print group information
#ifndef QT_NO_DEBUG
    fprintf(stderr, "Founded %d freq groups:\n", groupCount);
    for (short i = 0; i < groupCount; ++i)
    {
        fprintf(stderr, "%d. center %d Hz, gain %f dB, contain %d freq's:\n", i + 1,
                group[i].center,
                group[i].gain,
                group[i].freqCount);
        for (short j = 0; j < group[i].freqCount; ++j)
            fprintf(stderr, "\t%d. %d Hz\n", j + 1, group[i].freq[j]);
    }
#endif
    // Create filters
    for (short i = 0; i < groupCount; ++i)
        Hs_BiquadCalc(inst->filter + i, group + i);
    inst->filterCount = groupCount;
}

/*
 * Calulate Peak to Average Power Ration
 */
void Hs_EvaluatePAPR(float Y[], float PAPR[])
{
    // -- Energy
    float energy = 0,
          *Y_p = Y;
    while (Y_p < Y + HS_BLOCKL_MAX)
        energy += quad(*Y_p++);
    energy /= HS_BLOCKL_MAX;
    // -- Elements
    Y_p           = Y;
    float *PAPR_p = PAPR;
    while (Y_p < Y + HS_BLOCKL_MAX)
        *PAPR_p++ = 10 * log10f(quad(*Y_p++) / energy);
}

/*
 * Calulate Peak to Harmonic Power Ratio
 */
void Hs_EvaluatePHPR(float Y[], float PHPR[])
{
    for (short i = 0, i2; i < HS_BLOCKL_MAX; ++i)
    {
        PHPR[i] = 0;
        i2 = 2 * i; // m = 2
        if (i2 < HS_BLOCKL_MAX)
            PHPR[i] = 10 * log10f(quad(Y[i]) / quad(Y[i2]));

        i2 = 3 * i; // m = 3
        if (i2 < HS_BLOCKL_MAX)
        {
            float phprc = 10 * log10f(quad(Y[i]) / quad(Y[i2]));
            if (phprc > PHPR[i])
            PHPR[i] = phprc;
        }
    }
}

/*
 * Calulate Peak to Neighboring Power Ratio
 */
void Hs_EvaluatePNPR(float Y[], float PNPR[])
{
    static const int m[10] = {1, -1, 2, -2, 3, -3, 4, -4, 5, -5};
    for (short i = 0; i < HS_BLOCKL_MAX; ++i)
    {
        PNPR[i] = 0;
        for (short j = 0, i2; j < 10; ++j)
        {
            i2 = HS_RAD_TO_INDEX * (HS_INDEX_TO_RAD * i + 2 * PI_F * m[j] / HS_M);
            if (i2 < HS_BLOCKL_MAX)
                PNPR[i] += 10 * log10f(quad(Y[i]) / quad(Y[i2]));
        }
    }
}

/*
 * Find howling freques and write into instance array
 * returns: count of found freq
 */
int Hs_AnalyzeHowling(HsHandle *inst, short howlingFreq[], const short *input)
{
    float fin[HS_BLOCKL];
    // Convert input data to float
    for (short i = 0; i < HS_BLOCKL; ++i)
        fin[i] = (float) input[i];

    // Shift buffer
    memcpy(inst->dataBuf, inst->dataBuf + HS_BLOCKL,
           sizeof(float) * (HS_BLOCKL_MAX - HS_BLOCKL));     // Shift
    memcpy(inst->dataBuf + HS_BLOCKL_MAX - HS_BLOCKL, fin,
           sizeof(float) * HS_BLOCKL);                       // Copy new data

    // Apply window to new buffer
    float tmpBuf[HS_BLOCKL_MAX];
    for (short i = 0; i < HS_BLOCKL_MAX; ++i)
        tmpBuf[i] = inst->dataBuf[i] * kBlocks480w1024[i];

    // Apply FFT
    rdft(HS_BLOCKL_MAX, 1, tmpBuf, inst->ip, inst->wfft);

    // Calc criteries
    float papr[HS_BLOCKL_MAX],
          phpr[HS_BLOCKL_MAX],
          pnpr[HS_BLOCKL_MAX];
    Hs_EvaluatePAPR(tmpBuf, papr);
    Hs_EvaluatePHPR(tmpBuf, phpr);
    Hs_EvaluatePNPR(tmpBuf, pnpr);

    // Evaluate howling freq
    int   hFreqCount = 0, freq;
    for (short i = 0; i < HS_BLOCKL_MAX; ++i)
    {
        // TH check
        if ( papr[i] > inst->PAPR_TH &&
             phpr[i] > inst->PHPR_TH &&
             pnpr[i] > inst->PNPR_TH )
        {
            // Some magic freq converter from index
            freq = HS_INDEX_TO_HZ * i;
            // Skip freq when is out of MIN/MAX
            if (freq > HS_FREQ_MIN && freq < HS_FREQ_MAX)
            {
                howlingFreq[hFreqCount] = freq;
                ++hFreqCount;
                // Break when too many freq
                if (hFreqCount == HS_BIQUAD_COUNT)
                    break;
            }
        }
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

int Hs_Init(HsHandle *inst, int fs,
            float PAPR_TH, float PHPR_TH, float PNPR_TH)
{
    if (fs != 8000)
        return -1;

    // Initialize fft work arrays.
    inst->ip[0] = 0; // Setting this triggers initialization.
    memset(inst->dataBuf, 0, sizeof(float) * HS_BLOCKL_MAX);
    rdft(HS_BLOCKL_MAX, 1, inst->dataBuf, inst->ip, inst->wfft);
    memset(inst->dataBuf, 0, sizeof(float) * HS_BLOCKL_MAX);

    if (!PAPR_TH)
        inst->PAPR_TH = 10 * log10f(quad(HS_BLOCKL_MAX / 150));
    else
        inst->PAPR_TH = PAPR_TH;

    if (!PHPR_TH)
        inst->PHPR_TH = 33;
    else
        inst->PHPR_TH = PHPR_TH;

    if (!PNPR_TH)
        inst->PNPR_TH = 15;
    else
        inst->PNPR_TH = PNPR_TH;

#ifndef QT_NO_DEBUG
        fprintf(stderr, "HS_INIT: PAPR_TH = %f, PHPR_TH = %f, PNPR_TH = %f\n",
                PAPR_TH, PHPR_TH, PNPR_TH);
#endif
    return 0;
}

void Hs_Free(HsHandle *inst)
{
    free(inst);
}

void Hs_Process(HsHandle *inst, const short *input, short *output)
{
    short howlingFreq[HS_BIQUAD_COUNT];
    int freqCount = Hs_AnalyzeHowling(inst, howlingFreq, input);
    if (!freqCount && !inst->filterCount)
    {
        // Howling not found
        memcpy(output, input, HS_BLOCKL * sizeof(short));
        return;
    }

    // Print freq information
#ifndef QT_NO_DEBUG
    fprintf(stderr, "Detected %d hoOowling:\n", freqCount);
    for (short i = 0; i < freqCount; ++i)
        fprintf(stderr, "%d. %d Hz\n", i + 1, howlingFreq[i]);
#endif
    // Calc IIR filters
    Hs_BiquadUpdate(inst, howlingFreq, freqCount);

    // Print filter information
#ifndef QT_NO_DEBUG
    fprintf(stderr, "Enabled %d filters:\n", inst->filterCount);
    for (short i = 0; i < inst->filterCount; ++i)
        fprintf(stderr, "%d. freq %f Hz, gain %f dB, Q %f\n", i + 1,
                inst->filter[i].freq,
                inst->filter[i].peakGain,
                inst->filter[i].Q);
#endif
    // Apply IIR filter
    const short *input_p = input;
    while (input_p < input + HS_BLOCKL)
        *output++ = Hs_BiquadCascade(inst, *input_p++);
}
