#include "howling_suppression.h"
#include "windows_private.h"
#include "defines.h"
#include "fft4g.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

float quad(float a) { return a * a; }
float mod(float a[], int i) { return sqrtf(a[i] * a[i] + a[i+1] * a[i+1]); }

/*
 * Cascade filter applying
 */
float Hs_BiquadCascade(HsHandle *inst, float in)
{
    int count              = inst->filterCount;
    BiquadParams *filter   = inst->filter;
    while (count--)
        in = BiquadProcess(filter++, in);
    return in;
}

/*
 * Filter updater
 */
void Hs_BiquadUpdate(HsHandle *inst, short howlingFreq[], int freqCount)
{
    HsFreqGroup *group = inst->group;
    memset(group, 0, sizeof(HsFreqGroup) * HS_BIQUAD_COUNT);
    int groupCount = 0;
    // Enumerate howling freq's
    for (short i = 0; i < freqCount && groupCount < HS_BIQUAD_COUNT; ++i)
    {
        for (short j = 0; j  < groupCount && howlingFreq[i]; ++j)
            if (abs(howlingFreq[i] - inst->group[j].center) < HS_FREQ_DEVIATION)
            {
                if (group[j].freqCount < HS_GROUP_FREQ_MAX)
                {
                    group[j].freq[group[j].freqCount++] = howlingFreq[i];
                    group[j].center = Hs_FreqGroupCenter(group + j);
                }
                // Down freq
                howlingFreq[i] = 0;
            }
        for (short j = 0; j  < groupCount && howlingFreq[i]; ++j)
            if (abs(howlingFreq[i] - inst->group[j].center) < HS_FREQ_DEVIATION2)
            {
                group[groupCount].freqCount = 1;
                group[groupCount].freq[0]   = howlingFreq[i];
                group[groupCount].gain      = inst->group[j].gain;
                group[groupCount].center    = group[groupCount].freq[0];
                ++groupCount;
                // Down freq
                howlingFreq[i] = 0;
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
                if (group[j].freqCount < HS_GROUP_FREQ_MAX)
                {
                    group[j].freq[group[j].freqCount++] = inst->filter[i].freq;
                    group[j].center = Hs_FreqGroupCenter(group + j);
                }

                if (inst->filter[i].peakGain > HS_GAIN_MIN)
                    group[j].gain = inst->filter[i].peakGain - HS_GAIN_DOWN_STEP;
                else
                    group[j].gain = inst->filter[i].peakGain;

                founded = 1;
            }
        }
        if (!founded && inst->filter[i].peakGain + HS_GAIN_UP_STEP < 0)
        {
            group[groupCount].freqCount = 1;
            group[groupCount].freq[0] = inst->filter[i].freq;
            group[groupCount].center  = group[groupCount].freq[0];
            group[groupCount].gain    = inst->filter[i].peakGain + HS_GAIN_UP_STEP;
            ++groupCount;
        }
    }
    // Print group information
#ifdef HS_DEBUG
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
        BiquadCalcNotch(inst->filter + i, group + i);
    inst->filterCount = groupCount;
}

/*
 * Calulate Peak to Average Power Ration
 */
void Hs_EvaluatePAPR(float Y[], float PAPR[])
{
    // -- Energy
    float energy = 0;
    for (short i = 0; i < HS_BLOCKL_A; ++i)
        energy += quad(Y[i]);
    energy /= HS_BLOCKL_A;
    // -- Elements
    float *PAPR_p = PAPR;
    for (short i = 0; i < HS_BLOCKL_A; ++i)
        *PAPR_p++ = 10 * log10f(quad(Y[i]) / energy);
}

/*
 * Calulate Peak to Harmonic Power Ratio
 */
void Hs_EvaluatePHPR(float Y[], float PHPR[])
{
    for (short i = 0, i2; i < HS_BLOCKL_A; ++i)
    {
        PHPR[i] = HS_PHPR_UNDEF;
        i2 = 2 * i; // m = 2
        if (i2 < HS_BLOCKL_A)
            PHPR[i] = 10 * log10f(quad(Y[i]) / quad(Y[i2]));

        i2 = 3 * i; // m = 3
        if (i2 < HS_BLOCKL_A)
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
    for (short i = 0; i < HS_BLOCKL_A; ++i)
    {
        PNPR[i] = 0;
        for (short j = 0, i2; j < 10; ++j)
        {
            i2 = HS_RAD_TO_INDEX * (HS_INDEX_TO_RAD * i + 2 * PI_F * m[j] / HS_M);
            if (i2 < HS_BLOCKL_A)
                PNPR[i] += 10 * log10f(quad(Y[i]) / quad(Y[i2]));
        }
    }
}

/*
 * Calculate IMSD
 */
void Hs_EvaluateIMSD(float Y[], float IMSD[])
{
    const short Q_m = 16,
                P   = 1,
                t   = HS_BUF_COUNT - 1;
    float fst_sum, snd_sum;
    for (short i = 0; i < HS_BLOCKL_A; ++i)
    {
        IMSD[i] = 0;
        fst_sum = 0;
        for (short j = 0; j < Q_m; ++j)
            fst_sum += 20 * (log10f(Y[(t - j*P) * HS_BLOCKL_A + i])
                    - log10f(Y[(t - Q_m*P)*HS_BLOCKL_A + i])) / (Q_m - j);
        for (short m = 1; m < Q_m; ++m)
        {
            snd_sum = 0;
            for (short j = 0; j < m; ++j)
                snd_sum += 20 * (log10f(Y[(t - j*P) * HS_BLOCKL_A + i])
                                - log10f(Y[(t - m*P)*HS_BLOCKL_A + i])) / (m - j);
            IMSD[i] += fst_sum / Q_m - snd_sum / m;
        }
    }
}

/*
 * Find howling freques and write into instance array
 * returns: count of found freq
 */
int Hs_AnalyzeHowling(HsHandle *inst, short howlingFreq[], const float *input)
{
    float fin[HS_BLOCKL];
    // Convert input data to float and apply window
    for (short i = 0; i < HS_BLOCKL; ++i)
        fin[i] = input[i] * kBlackmanWindow256[i];

    // Apply RDFT
    rdft(HS_BLOCKL, 1, fin, inst->ip, inst->wfft);

    // Calc modulo
    for (short i = 0; i < HS_BLOCKL; i += 2)
        fin[i / 2] = mod(fin, i);

    // Shift RDFT buffer
    memcpy(inst->rdftBuf, inst->rdftBuf + HS_BLOCKL_A,
           sizeof(float) * (HS_BLOCKL_A_MAX - HS_BLOCKL_A));     // Shift
    memcpy(inst->rdftBuf + HS_BLOCKL_A_MAX - HS_BLOCKL_A, fin,
           sizeof(float) * HS_BLOCKL_A);                       // Copy new data

    // Calc criteries
    float papr[HS_BLOCKL_A],
          phpr[HS_BLOCKL_A],
          pnpr[HS_BLOCKL_A],
          imsd[HS_BLOCKL_A];
    Hs_EvaluatePAPR(fin, papr);
    Hs_EvaluatePHPR(fin, phpr);
    Hs_EvaluatePNPR(fin, pnpr);
    Hs_EvaluateIMSD(inst->rdftBuf, imsd);

#ifdef HS_DEBUG
    fprintf(stderr, "HS_TRASH: PAPR_TH = %f, PHPR_TH = %f, PNPR_TH = %f, IMSD = %f\n",
            inst->PAPR_TH, inst->PHPR_TH, inst->PNPR_TH, inst->IMSD_TH);
#endif

    // Evaluate howling freq
    int   hFreqCount = 0, freq;
    for (short i = 0; i < HS_BLOCKL_A; ++i)
    {
#ifdef HS_DEBUG
        const char *out;
        // Some magic freq converter from index
        freq = HS_INDEX_TO_HZ * i;
        fprintf(stderr, "Freq %d Hz >\t", freq);
        out = papr[i] > inst->PAPR_TH ? "+" : " ";
        fprintf(stderr, "PAPR : %.1f %s\t", papr[i], out);
        out = phpr[i] > inst->PHPR_TH ? "+" : " ";
        fprintf(stderr, "PHPR : %.1f %s\t", phpr[i], out);
        out = pnpr[i] > inst->PNPR_TH ? "+" : " ";
        fprintf(stderr, "PNPR : %.1f %s\t", pnpr[i], out);
        out = fabsf(imsd[i]) < inst->IMSD_TH ? "+" : " ";
        fprintf(stderr, "IMSD : %.1f %s\t", imsd[i], out);
        if ( papr[i] > inst->PAPR_TH &&
             phpr[i] > inst->PHPR_TH &&
             pnpr[i] > inst->PNPR_TH &&
             fabsf(imsd[i]) < inst->IMSD_TH)
            fprintf(stderr, " << FOUND >> ");
        fprintf(stderr, "\n");
#endif
        // TH check
        if ( papr[i] > inst->PAPR_TH &&
             phpr[i] > inst->PHPR_TH &&
             pnpr[i] > inst->PNPR_TH &&
             imsd[i] < inst->IMSD_TH)
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
      memset(*inst, 0, sizeof(HsHandle));
      return 0;
    } else {
      return -1;
    }
}

int Hs_Init(HsHandle *inst, int fs,
            float PAPR_TH, float PHPR_TH, float PNPR_TH, float IMSD_TH)
{
    if (fs != 8000)
        return -1;

    // Initialize fft work arrays.
    inst->ip[0] = 0; // Setting this triggers initialization.
    memset(inst->rdftBuf, 0, sizeof(float) * HS_BLOCKL_A_MAX);
    rdft(HS_BLOCKL, 1, inst->rdftBuf, inst->ip, inst->wfft);

    if (!PAPR_TH)
        inst->PAPR_TH = 10 * log10f(quad(HS_BLOCKL_A_MAX / 150));
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

    if (!IMSD_TH)
        inst->IMSD_TH = 0.5;
    else
        inst->IMSD_TH = IMSD_TH;

#ifdef HS_DEBUG
        fprintf(stderr, "HS_INIT: PAPR_TH = %f, PHPR_TH = %f, PNPR_TH = %f, IMSD = %f\n",
                PAPR_TH, PHPR_TH, PNPR_TH, IMSD_TH);
#endif
    return 0;
}

void Hs_Free(HsHandle *inst)
{
    free(inst);
}

void Hs_Process(HsHandle *inst, float *sample)
{
    short howlingFreq[HS_BIQUAD_COUNT];
    int freqCount = Hs_AnalyzeHowling(inst, howlingFreq, sample);
    if (!freqCount && !inst->filterCount)
    {
        // Howling not found
        return;
    }

    // Print freq information
#ifdef HS_DEBUG
    fprintf(stderr, "Detected %d hoOowling:\n", freqCount);
    for (short i = 0; i < freqCount; ++i)
        fprintf(stderr, "%d. %d Hz\n", i + 1, howlingFreq[i]);
#endif
    // Calc IIR filters
    Hs_BiquadUpdate(inst, howlingFreq, freqCount);

    // Print filter information
#ifdef HS_DEBUG
    fprintf(stderr, "Enabled %d filters:\n", inst->filterCount);
    for (short i = 0; i < inst->filterCount; ++i)
        fprintf(stderr, "%d. freq %f Hz, gain %f dB, Q %f\n", i + 1,
                inst->filter[i].freq,
                inst->filter[i].peakGain,
                inst->filter[i].Q);
#endif
    // Apply IIR filter
    float *input_p = sample;
    while (input_p < sample + HS_BLOCKL)
    {
        *input_p = Hs_BiquadCascade(inst, *input_p);
        ++input_p;
    }
}
