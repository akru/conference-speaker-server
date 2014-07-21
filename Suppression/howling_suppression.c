#include "howling_suppression.h"
#include "windows_private.h"
#include "defines.h"
#include "fft4g.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

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
void Hs_BiquadUpdate(HsHandle *inst, int freqCount)
{
    HsBiquadParams filter[HS_BIQUAD_COUNT];
    short filterCount = 0,
          freqId;
    // Update exist filters
    for (short i = 0; i < inst->filterCount; ++i)
    {
        freqId = deviationCheck(inst->filter[i].freq, inst->howlingFreq, freqCount);
        if (freqId < 0)
        {
            if (inst->filter[i].peakGain + HS_GAIN_STEP < 0)
            {
                filter[filterCount] = inst->filter[i];
                filter[filterCount].peakGain += HS_GAIN_STEP;
                Hs_BiquadCalc(filter + filterCount);
                // Increase count of filters
                ++filterCount;
            }
        }
        else
        {
            filter[filterCount] = inst->filter[i];
            if (filter[filterCount].peakGain > HS_GAIN_MIN)
            {
                filter[filterCount].peakGain -= HS_GAIN_STEP;
                Hs_BiquadCalc(filter + filterCount);
            }
            // Increase count of filters
            ++filterCount;
            // Down freq
            inst->howlingFreq[freqId] = 0;
        }
    }
    // Create unexist filters
    for (short i = 0; i < freqCount && filterCount < HS_BIQUAD_COUNT; ++i)
    {
        if (inst->howlingFreq[i])
        {
            Hs_BiquadInit(filter + filterCount);
            filter[filterCount].freq = inst->howlingFreq[i];
            Hs_BiquadCalc(filter + filterCount);
            // Increase count of filters
            ++filterCount;
        }
    }
    // Copy filter to inst
    memcpy(inst->filter, filter, sizeof(HsBiquadParams) * filterCount);
    inst->filterCount = filterCount;
}

/*
 * Find howling freques and write into instance array
 * returns: count of found freq
 */
int Hs_AnalyzeHowling(HsHandle *inst, const short *input)
{
    float fin[HS_BLOCKL];
    float tmpBuf[HS_BLOCKL_MAX];

    // Convert input data to float
    for (short i = 0; i < HS_BLOCKL; ++i)
    {
        fin[i] = (float) input[i];
    }

    // Update buffers
    memcpy(inst->dataBuf, inst->dataBuf + HS_BLOCKL,
           sizeof(float) * (HS_BLOCKL_MAX - HS_BLOCKL));     // Shift
    memcpy(inst->dataBuf + HS_BLOCKL_MAX - HS_BLOCKL, fin,
           sizeof(float) * HS_BLOCKL);                       // Copy new data

    // Apply window TODO: overlap
    float *tmpBuf_p  = tmpBuf,
          *dataBuf_p = inst->dataBuf;
    const float
          *window_p  = kBlocks480w1024;
    while (tmpBuf_p < tmpBuf + HS_BLOCKL_MAX)
        *tmpBuf_p++ = *dataBuf_p++ * *window_p++;

    // Apply FFT
    rdft(HS_BLOCKL_MAX, 1, tmpBuf, inst->ip, inst->wfft);

    // Calc Peak to Average Power Ration
    float papr[HS_BLOCKL_MAX];
    // -- Energy
    tmpBuf_p     = tmpBuf;
    float energy = 0;
    while (tmpBuf_p < tmpBuf + HS_BLOCKL_MAX)
        energy += quad(*tmpBuf_p++);
    energy /= HS_BLOCKL_MAX;
    // -- Elements
    tmpBuf_p     = tmpBuf;
    float *papr_p= papr;
    while (tmpBuf_p < tmpBuf + HS_BLOCKL_MAX)
        *papr_p++ = 10 * log10f(quad(*tmpBuf_p++) / energy);
    // -- Trashold
    const float papr_trashold = 10 * log10f(quad(HS_BLOCKL_MAX / 150));

    // Calc Peak to Harmonic Power Ratio
    float phpr[HS_BLOCKL_MAX], phprc;
    // -- Elements
    for (short i = 0, i2; i < HS_BLOCKL_MAX; ++i)
    {
        phpr[i] = 0;
        i2 = 2 * i; // m = 2
        if (i2 < HS_BLOCKL_MAX)
            phpr[i] = 10 * log10f(quad(tmpBuf[i]) / quad(tmpBuf[i2]));

        i2 = 3 * i; // m = 3
        if (i2 < HS_BLOCKL_MAX)
        {
            phprc = 10 * log10f(quad(tmpBuf[i]) / quad(tmpBuf[i2]));
            if (phprc > phpr[i])
                phpr[i] = phprc;
        }
    }
    // -- Trashold
    const float phpr_trashold = 33; // dB

    // Calc Peak to Neighboring Power Ratio
    const int m[10] = {1, -1, 2, -2, 3, -3, 4, -4, 5, -5};
    float pnpr[HS_BLOCKL_MAX];
    // -- Elements
    for (short i = 0; i < HS_BLOCKL_MAX; ++i)
    {
        pnpr[i] = 0;
        for (short j = 0, i2; j < 10; ++j)
        {
            i2 = HS_RAD_TO_INDEX * (HS_INDEX_TO_RAD * i + 2 * PI_F * m[j] / HS_M);
            if (i2 < HS_BLOCKL_MAX)
                pnpr[i] += 10 * log10f(quad(tmpBuf[i]) / quad(tmpBuf[i2]));
        }
    }
    // -- Trashold
    const float pnpr_trashold = 15; // dB

    // Evaluate howling freq
    int   hFreqCount = 0, freq;
    for (short i = 0; i < HS_BLOCKL_MAX; ++i)
    {
        // TH check
        if (papr[i] > papr_trashold &&
            phpr[i] > phpr_trashold &&
            pnpr[i] > pnpr_trashold)
        {
            freq = // Some magic freq converter from index
                   HS_INDEX_TO_HZ * i;
            // Skip freq when deviation is small
            if (freq > HS_FREQ_MIN && freq < HS_FREQ_MAX &&
                    (!hFreqCount ||
                     0 > deviationCheck(freq, inst->howlingFreq, hFreqCount)))
            {
                inst->howlingFreq[hFreqCount] = freq;
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

int Hs_Init(HsHandle *inst, int fs)
{
    if (fs != 8000)
        return -1;

    // Initialize fft work arrays.
    inst->ip[0] = 0; // Setting this triggers initialization.
    memset(inst->dataBuf, 0, sizeof(float) * HS_BLOCKL_MAX);
    rdft(HS_BLOCKL_MAX, 1, inst->dataBuf, inst->ip, inst->wfft);
    memset(inst->dataBuf, 0, sizeof(float) * HS_BLOCKL_MAX);

    return 0;
}

void Hs_Free(HsHandle *inst)
{
    free(inst);
}

void Hs_Process(HsHandle *inst, const short *input, short *output)
{
    fprintf(stderr, "HsProcess started\n");
    int freqCount = Hs_AnalyzeHowling(inst, input);
    if (!freqCount && !inst->filterCount)
    {
        // Howling not found
        memcpy(output, input, HS_BLOCKL * sizeof(short));
        return;
    }

    // Print freq information
    fprintf(stderr, "Detected %d hoOowling:\n", freqCount);
    for (short i = 0; i < freqCount; ++i)
        fprintf(stderr, "%d. %d Hz\n", i + 1, inst->howlingFreq[i]);

    // Calc IIR filters
    Hs_BiquadUpdate(inst, freqCount);

    // Print filter information
    fprintf(stderr, "Enabled %d filters:\n", inst->filterCount);
    for (short i = 0; i < inst->filterCount; ++i)
        fprintf(stderr, "%d. freq %f Hz, gain %f dB\n", i + 1,
                inst->filter[i].freq,
                inst->filter[i].peakGain);

    // Apply IIR filter
    const short *input_p = input;
    while (input_p < input + HS_BLOCKL)
        *output++ = Hs_BiquadCascade(inst, *input_p++);
    fprintf(stderr, "HsProcess ended\n");
}
