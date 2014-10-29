#include "equalizer_filter.h"

#include <cmath>
#include <QDebug>
#include <QSettings>
#include <soxr.h>
#include <QTime>

#define B_SPLINE_EQ

static const quint32 bands[8] = {
    Equalizer::hz_to_index * 0,
    Equalizer::hz_to_index * 250,
    Equalizer::hz_to_index * 500,
    Equalizer::hz_to_index * 1000,
    Equalizer::hz_to_index * 3000,
    Equalizer::hz_to_index * 5000,
    Equalizer::hz_to_index * 8000,
    Equalizer::hz_to_index * 10000
};


/*
 * Interpolate samples.
 * Params:
 *  samples - output interpolated array
 *  length - samples length
 *  xs, ys - interpolation input
 *  n_points - input arrays length
 */
void interpolate(float *samples, quint16 length,
                 const quint32 *xs, const float *ys, quint16 n_points)
{
#ifdef B_SPLINE_EQ // B-Spline interpolation
    float dist, span;
    float value = 0.0;
    int minF = 0;

    for(quint16 i = 0; i < length; ++i)
    {
        while ((xs[minF] <= i) && (minF < n_points))
            ++minF;
        --minF;
        if (minF < 0) //before first slider
        {
            dist = xs[0] - i;
            span = xs[1] - xs[0];
            if( dist < span )
                value = ys[0]*(1. + cos(M_PI*dist/span))/2.;
            else
                value = 0.;
        }
        else
        {
            if(i > xs[n_points-1])   //after last fader
            {
                span = xs[n_points-1] - xs[n_points-2];
                dist = i - xs[n_points-1];
                value = ys[n_points-1]*(1. + cos(M_PI*dist/span))/2.;
            }
            else  //normal case
            {
                span = xs[minF+1] - xs[minF];
                dist = xs[minF+1] - i;
                value = ys[minF]*(1. + cos(M_PI*(span-dist)/span))/2. +
                        ys[minF+1]*(1. + cos(M_PI*dist/span))/2.;
            }
        }
        samples[i] = value;
    }
#else // Bilinear interpolation
    /* Note that xs must be monotonically increasing! */
    /* Description: x - frequency, y - scale */

    float x_range_lower, x_range_upper, c0;

    Q_ASSERT(n_points >= 2);
    Q_ASSERT(xs[0] == 0);
    Q_ASSERT(xs[n_points - 1] == length - 1);

    for (size_t x = 0, x_range_lower_i = 0; x < length-1; ++x) {
        Q_ASSERT(x_range_lower_i < n_points-1);

        x_range_lower = (float) xs[x_range_lower_i];
        x_range_upper = (float) xs[x_range_lower_i+1];

        Q_ASSERT(x_range_lower < x_range_upper);
        Q_ASSERT(x >= x_range_lower);
        Q_ASSERT(x <= x_range_upper);

        /* bilinear-interpolation of coefficients specified */
        c0 = (x-x_range_lower) / (x_range_upper-x_range_lower);
        Q_ASSERT(c0 >= 0 && c0 <= 1.0);

        samples[x] = ((1.0f - c0) * ys[x_range_lower_i] + c0 * ys[x_range_lower_i + 1]);
        while(x >= xs[x_range_lower_i + 1])
            x_range_lower_i++;
    }

    samples[length-1] = ys[n_points-1];
#endif
}

EqualizerFilter::EqualizerFilter(float X, const float *W)
    : first_iteration(true),
      X(X / Equalizer::fft_size * 2), W(W)
{
    memset(overlap, 0, sizeof(float) * Equalizer::overlap_size);
    memset(buffer,  0, sizeof(float) * Filter::sample_length * 2);

    // Zero padding compensation with resampler
    soxr_error_t error;
//    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);
    soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_HQ, 0);
    resampler = soxr_create(Filter::sample_rate * Equalizer::zero_scaler, Filter::sample_rate,
                            1, &error, NULL, &q_spec, NULL);
    if (error) {
        qWarning() << "SoX has an error: " << error;
        return;
    }

    reloadSettings();
}

EqualizerFilter::~EqualizerFilter()
{
}

void EqualizerFilter::reloadSettings()
{
    QSettings s(settingsFiltename(), QSettings::IniFormat);
    enable(s.value("eq-enable", true).toBool());
    setUserBand(s.value("eq-slider-1", 50).toInt(),
                 s.value("eq-slider-2", 50).toInt(),
                 s.value("eq-slider-3", 50).toInt(),
                 s.value("eq-slider-4", 50).toInt(),
                 s.value("eq-slider-5", 50).toInt(),
                 s.value("eq-slider-6", 50).toInt(),
                 s.value("eq-slider-7", 50).toInt());
    qDebug() << "Equalize settings reloaded";
}

void EqualizerFilter::setUserBand(short B1, short B2, short B3,
                                  short B4, short B5, short B6, short B7)
{
    float uBandH[Equalizer::h_size * Equalizer::zero_scaler], ys[8];
    /* Static indexes for 16kHz freq */
    const quint32 *xs = bands;
    static const float scale = 0.02f;
    // Set six bands coefs
    ys[0] = 0;
    ys[1] = B1 * scale; ys[2] = B2 * scale; ys[3] = B3* scale;
    ys[4] = B4 * scale; ys[5] = B5 * scale; ys[6] = B6 * scale;
    ys[7] = B7 * scale;
    // Interpolate values
    interpolate(uBandH, Equalizer::h_size * Equalizer::zero_scaler, xs, ys, 8);
    // Debug output
//    qDebug() << "B: " << B1 << B2 << B3 << B4 << B5 << B6 << B7;
    for (short i = 0; i < Equalizer::h_size * Equalizer::zero_scaler; ++i)
        qDebug() << "H [" << i << "] =" << uBandH[i];
//        qDebug() << uBandH[i];
    // Set new H
    setFullBand(uBandH);
}

void EqualizerFilter::setFullBand(const float fbH[])
{
    // Copy new H
    if (fbH)
        memcpy(H, fbH, Equalizer::h_size * Equalizer::zero_scaler* sizeof(float));
    else
        for (short i = 0; i < Equalizer::h_size * Equalizer::zero_scaler; ++i)
            H[i] = 1;
}

void EqualizerFilter::processFilter(float sample[])
{
    if (first_iteration)
    {
        first_iteration = false;
        // Copy to input buffer
        memcpy(buffer, sample, Filter::sample_length * sizeof(float));
    }
    else
    {
        QTime t = QTime::currentTime();
        // Copy to input buf
        memcpy(buffer + Filter::sample_length, sample, Filter::sample_length * sizeof(float));
        // Process pass1
        dsp_logic();
        //preserve the needed input for the next window's overlap
        memcpy(sample, buffer, Equalizer::R * sizeof(float));
        memmove(buffer, buffer + Equalizer::R, (Filter::sample_length * 2 - Equalizer::R) * sizeof(float));
        // Process pass2
        dsp_logic();
        //preserve the needed input for the next window's overlap
        memcpy(sample + Equalizer::R, buffer, Equalizer::overlap_size * sizeof(float));
        memmove(buffer, buffer + Equalizer::R, (Filter::sample_length * 2 - Equalizer::R) * sizeof(float));
        qDebug() << "EQ Elapsed" << t.elapsed() << "ms";
    }
}

void EqualizerFilter::dsp_logic()
{
    //use a linear-phase sliding STFT and overlap-add method (for each channel)

    float output_window[Equalizer::fft_size];
    //window the data
    for(short j = 0; j < Equalizer::window_size; ++j) {
        output_window[j] = X * W[j] * buffer[j];
    }
    //zero pad the remaining fft window
    memset(output_window + Equalizer::window_size, 0,
          (Equalizer::fft_size - Equalizer::window_size) * sizeof(float));

    //Processing is done here!
    //do fft
    cdftf(Equalizer::fft_size, 1, output_window);

    //perform filtering
    for(short j = 0; j < Equalizer::fft_size; j+=2) {
        output_window[j]   *= H[j/2];
        output_window[j+1] *= H[j/2];
//        qDebug() << "H["<<j/2<<"]="<<H[j/2];
    }
    //inverse fft
    cdftf(Equalizer::fft_size, -1, output_window);
    // #TODO: NORMING

    // Resampling for compensate zero padding
    size_t idone, odone;
    float resampled_window[Equalizer::window_size];
    soxr_process(resampler,
                 output_window,     Equalizer::fft_size,     &idone,
                 resampled_window,  Equalizer::window_size,  &odone);
    qDebug() << "idone, odone:" << idone << "," << odone;

    for(short i = 0; i < Equalizer::window_size; i++){
        if(resampled_window[i] != resampled_window[i]) {
            resampled_window[i] = 0.;
            qDebug() << "Got a NaN after resampling at EQ!";
        }
    }

    //overlap add and preserve overlap component from this window (linear phase)
    for(short j = 0; j < Equalizer::overlap_size; ++j) {
        buffer[j] = resampled_window[j] + overlap[j];
        overlap[j] = resampled_window[Equalizer::R + j];
    }

}
