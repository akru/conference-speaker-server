#include "hs_filter.h"

#include <soxr.h>
#include <cstdio>
#include <QDebug>
#include <QSettings>

HSFilter::HSFilter(EqualizerFilter *eq)
    : eq(eq)
{
    Q_ASSERT(eq);
    // Initialize fft work array
    memset(dft_buf, 0, sizeof(float) * HS::dft_buffer_len);
#ifdef HS_DEBUG
    qDebug() << "HS_INIT: PAPR_TH =" << PAPR_TH
             << "PHPR_TH =" << PHPR_TH
             << "PNPR_TH =" << PNPR_TH
             << "IMSD =" << IMSD_TH;
#endif
    reloadSettings();
}

HSFilter::~HSFilter()
{
}

void HSFilter::reloadSettings()
{
    QSettings s(settingsFiltename(), QSettings::IniFormat);
    enable(s.value("hs-enable", true).toBool());
    setTH(s.value("hs-papr", 15).toFloat(),
          s.value("hs-phpr", 15).toFloat(),
          s.value("hs-pnpr", 15).toFloat(),
          s.value("hs-ismd", 0.03).toFloat());
}

void HSFilter::processFilter(float sample[])
{
    short howling_freq[HS::freq_count_max];
    int freq_count = analyze_howling(howling_freq, sample);
    if (!freq_count)
    {
        // Howling not found
        return;
    }
    // Print freq information
#ifdef HS_DEBUG
    qDebug() << "Detected" << freq_count << "hoOowling:";
    for (short i = 0; i < freq_count; ++i)
        qDebug() << i + 1 << ">" << howling_freq[i] << "Hz";
#endif
    // Fix equalizer by hoowling
    float *H = eq->getFullBand();
//    float bandwidth_hz = 0;
//    float bandwidth_oct = 1/60; // should be configurable from GUI: 1/60, 2/60, 3/60, 5/60
    for (short i = 0; i < freq_count; ++i)
    {
//        bandwidth_hz = ( (pow(2, bandwidth_oct)-1) / pow(2, bandwidth_oct/2) )*howling_freq[i];

        // interpolate
        // apply

        int ix = (int) (howling_freq[i] * Equalizer::hz_to_index);
        if (H[ix] > HS::filter_step)
            H[ix] -= HS::filter_step;
        else
            H[ix] = 0;
    }

    // For each freq in freq_count
    // find interval
    // interpolate in interval with current center freq value
    // apply it to H
}
/*
 * Find howling freques and write into instance array
 * returns: count of found freq
 */
short HSFilter::analyze_howling(short howling_freq[], const float input[])
{
    float fin[HS::fft_input_length];
    // Convert input data to float and apply window
    for (short i = 0; i < Filter::sample_length; ++i)
        fin[i] = input[i] * kBlackmanWindow512[i];
    memset(fin + Filter::sample_length, 0,
           sizeof(float) * (HS::fft_input_length - Filter::sample_length));

    // Apply RDFT
    rdftf(HS::fft_input_length, 1, fin);

    // Calc modulo
    for (short i = 0; i < HS::fft_input_length; i += 2)
        fin[i / 2] = sqrtf(fin[i] * fin[i] + fin[i+1] * fin[i+1]);

    // Shift RDFT buffer
    memcpy(dft_buf, dft_buf + HS::analyze_length,
           sizeof(float) * (HS::dft_buffer_len - HS::analyze_length));     // Shift
    memcpy(dft_buf + HS::dft_buffer_len - HS::analyze_length, fin,
           sizeof(float) * HS::analyze_length);                       // Copy new data

    // Calc criteries
    float papr[HS::analyze_length],
          phpr[HS::analyze_length],
          pnpr[HS::analyze_length],
          imsd[HS::analyze_length];
    evaluatePAPR(fin, papr);
    evaluatePHPR(fin, phpr);
    evaluatePNPR(fin, pnpr);
    evaluateIMSD(dft_buf, imsd);

    // Evaluate howling freq
    short freq_count = 0, freq;
    for (short i = 0; i < HS::analyze_length; ++i)
    {
#ifdef HS_DEBUG
        qDebug() << "Freq" << (int) (HS::index_to_hz * i) << "Hz >"
                 << "PAPR :" << papr[i] << (papr[i] > PAPR_TH ? "+" : " ")
                 << "PHPR :" << papr[i] << (phpr[i] > PHPR_TH ? "+" : " ")
                 << "PNPR :" << pnpr[i] << (pnpr[i] > PNPR_TH ? "+" : " ")
                 << "IMSD :" << imsd[i] << (fabs(imsd[i]) < IMSD_TH ? "+" : " ")
                 << (( papr[i] > PAPR_TH &&
                       phpr[i] > PHPR_TH &&
                       pnpr[i] > PNPR_TH &&
                       fabsf(imsd[i]) < IMSD_TH) ? "\t<< FOUND >>" : "");
#endif
        // TH check
        if ( papr[i] > PAPR_TH
             && phpr[i] > PHPR_TH
             //&& pnpr[i] > PNPR_TH
             && fabs(imsd[i]) < IMSD_TH)
        {
            // Some magic freq converter from index
            freq = HS::index_to_hz * i;
            // Skip freq when is out of MIN/MAX
            if (freq > HS::minimal_freq && freq < HS::maximal_freq)
            {
                howling_freq[freq_count] = freq;
                ++freq_count;
                // Break when too many freq
                if (freq_count == HS::freq_count_max)
                    break;
            }
        }
    }
    return freq_count;
}

/*
 * Calulate Peak to Average Power Ration
 */
void HSFilter::evaluatePAPR(const float Y[], float PAPR[])
{
    // -- Energy
    float energy = 0;
    for (short i = 0; i < HS::analyze_length; ++i)
        energy += Y[i] * Y[i];
    energy /= HS::analyze_length;
    // -- Elements
    float *PAPR_p = PAPR;
    for (short i = 0; i < HS::analyze_length; ++i)
        *PAPR_p++ = 10 * log10f(Y[i] * Y[i] / energy);
}

/*
 * Calulate Peak to Harmonic Power Ratio
 */
void HSFilter::evaluatePHPR(const float Y[], float PHPR[])
{
    for (short i = 0, i2; i < HS::analyze_length; ++i)
    {
        PHPR[i] = 100;
        i2 = 2 * i; // m = 2
        if (i2 < HS::analyze_length)
            PHPR[i] = 10 * log10f(Y[i] * Y[i] / Y[i2] / Y[i2]);

        i2 = 3 * i; // m = 3
        if (i2 < HS::analyze_length)
        {
            float phprc = 10 * log10f(Y[i] * Y[i] / Y[i2] / Y[i2]);
            if (phprc > PHPR[i])
                PHPR[i] = phprc;
        }
    }
}

/*
 * Calulate Peak to Neighboring Power Ratio
 */
void HSFilter::evaluatePNPR(const float Y[], float PNPR[])
{
    static const int m[10] = {1, -1, 2, -2, 3, -3, 4, -4, 5, -5};
    for (short i = 0; i < HS::analyze_length; ++i)
    {
        PNPR[i] = 0;
        for (short j = 0, i2; j < 10; ++j)
        {
            i2 = HS::rad_to_index * (HS::index_to_rad * i + 2 * M_PI * m[j] / HS::analyze_length);
            if (i2 < HS::analyze_length)
                PNPR[i] += 10 * log10f(Y[i] * Y[i] / Y[i2] / Y[i2]);
        }
    }
}

/*
 * Calculate IMSD
 */
void HSFilter::evaluateIMSD(const float Y[], float IMSD[])
{
    const short Q_m = 4,
                P   = 1,
                t   = HS::dft_buffer_count - 1;
    float fst_sum, snd_sum;
    for (short i = 0; i < HS::analyze_length; ++i)
    {
        IMSD[i] = 0;
        fst_sum = 0;
        for (short j = 0; j < Q_m; ++j)
            fst_sum += 20 * (log10f(Y[(t - j*P) * HS::analyze_length + i])
                    - log10f(Y[(t - Q_m*P) * HS::analyze_length + i])) / (Q_m - j);
        for (short m = 1; m < Q_m; ++m)
        {
            snd_sum = 0;
            for (short j = 0; j < m; ++j)
                snd_sum += 20 * (log10f(Y[(t - j*P) * HS::analyze_length + i])
                                - log10f(Y[(t - m*P) * HS::analyze_length + i])) / (m - j);
            IMSD[i] += fst_sum / Q_m - snd_sum / m;
        }
    }
}
