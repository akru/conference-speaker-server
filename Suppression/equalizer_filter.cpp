#include "equalizer_filter.h"
#include "fft4g.h"

#include <QDebug>

EqualizerFilter::EqualizerFilter(float X, const float *H, const float *W)
    : first_iteration(true),
      X(X), H(H), W(W)
{
    memset(overlap, 0, sizeof(float) * overlap_size);
    memset(buffer,  0, sizeof(float) * sample_length * 2);
    memset(ip, 0, (fft_size * 2 >> 1) * sizeof(float));
    memset(wfft, 0, (fft_size * 2 >> 1) * sizeof(float));
    //cdft(fft_size, 1, buffer, ip, wfft);
}

EqualizerFilter::~EqualizerFilter()
{
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
