#include "equalizer_filter.h"
#include "fft4g.h"

#define FILTER_SIZE(u) (u->fft_size / 2 + 1)
#define FLT_EPSILON	__FLT_EPSILON__

EqualizerFilter::EqualizerFilter(float X, const float *H, const float *W)
    : first_iteration(true),
      X(X), H(H), W(W),
      overlap(new float[overlap_size]),
      buffer(new float[sample_length * 2])
{
    memset(overlap, 0, sizeof(float) * overlap_size);
    memset(buffer,  0, sizeof(float) * sample_length * 2);
}

EqualizerFilter::~EqualizerFilter()
{
    delete [] overlap;
    delete [] buffer;
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
        memmove(buffer, buffer + R, (window_size - R) * sizeof(float));
        // Process pass2
        dsp_logic();
        //preserve the needed input for the next window's overlap
        memcpy(sample + R, buffer, overlap_size * sizeof(float));
        memmove(buffer, buffer + R, (window_size - R) * sizeof(float));
    }
}

void EqualizerFilter::dsp_logic()
{
    float wfft[fft_size * 2 >> 1],
          output_window[fft_size];
    int   ip[fft_size * 2 >> 1]; ip[0] = 0;
    //use a linear-phase sliding STFT and overlap-add method (for each channel)
    //window the data
    for(short j = 0; j < window_size; ++j)
        output_window[j] = X * W[j] * buffer[j];
    //zero pad the remaining fft window
//    memset(work_buffer + window_size, 0, (fft_size - window_size) * sizeof(float));
    //Processing is done here!
    //do fft
    cdft(fft_size, 1, output_window, ip, wfft);
    //perform filtering
    for(size_t j = 0; j < FILTER_SIZE(this); ++j) {
        output_window[j] *= H[j];
        output_window[j+1] *= H[j];
    }
    //inverse fft
    cdft(fft_size, -1, output_window, ip, wfft);
    ////debug: tests overlapping add
    ////and negates ALL PREVIOUS processing
    ////yields a perfect reconstruction if COLA is held
    //for(size_t j = 0; j < u->window_size; ++j) {
    //    u->work_buffer[j] = u->W[j] * u->input[c][j];
    //}

    //overlap add and preserve overlap component from this window (linear phase)
    for(short j = 0; j < overlap_size; ++j) {
        buffer[j] = output_window[j] + overlap[j];
        overlap[j] = buffer[R + j];
    }
    ////debug: tests if basic buffering works
    ////shouldn't modify the signal AT ALL (beyond roundoff)
    //for(size_t j = 0; j < u->window_size;++j) {
    //    u->work_buffer[j] = u->input[c][j];
    //}
}
