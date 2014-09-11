#include "equalizer_filter.h"
#include "fft4g.h"

#include <QDebug>

void interpolate(float *samples,
                 size_t length,
                 quint32 *xs,
                 float *ys,
                 size_t n_points) {
    /* Note that xs must be monotonically increasing! */
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
}

EqualizerFilter::EqualizerFilter(float X, const float *fbH, const float *W)
    : first_iteration(true),
      X(X / fft_size), W(W)
{
    memset(overlap, 0, sizeof(float) * overlap_size);
    memset(buffer,  0, sizeof(float) * sample_length * 2);
    memset(ip, 0, (fft_size * 2 >> 1) * sizeof(float));
    memset(wfft, 0, (fft_size * 2 >> 1) * sizeof(float));

    setFullBand(fbH);
}

EqualizerFilter::~EqualizerFilter()
{
}

void EqualizerFilter::setSixBand(short B1, short B2, short B3,
                                 short B4, short B5, short B6)
{
    float sixBandH[fft_size], ys[7];
    /* Static indexes for 16kHz freq */
    static quint32 xs[7] = { 0, 4, 8, 16, 48, 80, 128 };
    static const float scale = 0.1;
    // Set six bands coefs
    ys[0] = 0;
    ys[1] = (B1 - 50) * scale; ys[2] = (B2 - 50) * scale; ys[3] = (B3 - 50) * scale;
    ys[4] = (B4 - 50) * scale; ys[5] = (B5 - 50) * scale; ys[6] = (B6 - 50) * scale;
    // Interpolate values
    interpolate(sixBandH, fft_size, xs, ys, 7);
    // Debug output
    qDebug() << "B: " << B1 << B2 << B3 << B4 << B5 << B6;
    for (short i = 0; i < fft_size; ++i)
        qDebug() << "H [" << i << "] =" << H[i];
    // Set new H
    setFullBand(sixBandH);
}

void EqualizerFilter::setFullBand(const float fbH[])
{
    // Copy new H
    if (fbH)
        memcpy(H, fbH, fft_size * sizeof(float));
    else
        for (short i = 0; i < fft_size; ++i)
            H[i] = 1;
}

void EqualizerFilter::process(float sample[])
{
    if (first_iteration)
    {
        first_iteration = false;
        // Copy to input buffer
        memcpy(buffer, sample, sample_length * sizeof(float));
    }
    else
    {
        // Copy to input buf
        memcpy(buffer + sample_length, sample, sample_length * sizeof(float));
        // Process pass1
        dsp_logic();
        //preserve the needed input for the next window's overlap
        memcpy(sample, buffer, R * sizeof(float));
        memmove(buffer, buffer + R, (sample_length * 2 - R) * sizeof(float));
        // Process pass2
        dsp_logic();
        //preserve the needed input for the next window's overlap
        memcpy(sample + R, buffer, overlap_size * sizeof(float));
        memmove(buffer, buffer + R, (sample_length * 2 - R) * sizeof(float));
    }
}

void EqualizerFilter::dsp_logic()
{
//    qDebug() << "DSP: X=" << X;
//    for (short i =0; i < sample_length; ++i)
//        qDebug() << "H[" << i << "]=" << H[i];

    float output_window[fft_size];
    //use a linear-phase sliding STFT and overlap-add method (for each channel)
    //window the data
    for(short j = 0; j < window_size; ++j) {
//        qDebug() << "B[" << j << "]=" << buffer[j];
        output_window[j] = X * W[j] * buffer[j];
//        qDebug() << "W[" << j << "]=" << output_window[j];
    }
    //zero pad the remaining fft window
//    memset(work_buffer + window_size, 0, (fft_size - window_size) * sizeof(float));
    //Processing is done here!
    //do fft
    rdft(fft_size, 1, output_window, ip, wfft);
    //perform filtering
    for(short j = 0; j < fft_size / 2 - 1; ++j) {
        output_window[j]   *= H[j];
        output_window[j+1] *= H[j];
    }
    //inverse fft
    rdft(fft_size, -1, output_window, ip, wfft);
    ////debug: tests overlapping add
    ////and negates ALL PREVIOUS processing
    ////yields a perfect reconstruction if COLA is held
    //for(size_t j = 0; j < u->window_size; ++j) {
    //    u->work_buffer[j] = u->W[j] * u->input[c][j];
    //}

    //overlap add and preserve overlap component from this window (linear phase)
    for(short j = 0; j < overlap_size; ++j) {
        buffer[j] = output_window[j] + overlap[j];
        overlap[j] = output_window[R + j];

//        qDebug() << "W2[" << j << "]=" << output_window[j];
    }
    ////debug: tests if basic buffering works
    ////shouldn't modify the signal AT ALL (beyond roundoff)
    //for(size_t j = 0; j < u->window_size;++j) {
    //    u->work_buffer[j] = u->input[c][j];
    //}
}
