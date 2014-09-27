#ifndef EQUALIZER_FILTER_H
#define EQUALIZER_FILTER_H

#include "../Speaker/filter.h"
#include "windows_private.h"

class EqualizerFilter : public Filter
{
public:
    EqualizerFilter(float X=1,                          // Multiplier
                    const float *fbH=0,                 // The freq. magnitude scalers filter
                    const float *W=kBlackmanWindow512); // The windowing function
    ~EqualizerFilter();

    inline void setMultiplier(float x) { X = x / fft_size; }
    /*
     * Input: six values from 0 to 100 for bands:
     * 250Hz, 500Hz, 1kHz, 3kHz, 5kHz, 8kHz, 10kHz
     * and 16kHz sample rate
     */
    void setUserBand(short B1, short B2, short B3,
                     short B4, short B5, short B6, short B7);
    /*
     * Bans step is 43Hz
     */
    void setFullBand(const float fbH[]);
    float * getFullBand() { return H; }

    void process(float sample[]);
    QString name() { return "Equalizer"; }

    static const int window_size  = sample_length;
    static const int R            = window_size / 2;
    static const int overlap_size = window_size - R;
    static const int fft_size     = sample_length;
    static const int h_size       = fft_size / 2 + 1;

    // Freq to index converter for H
    static const float hz_to_index = 2.0 * h_size / sample_rate;

private:

    bool  first_iteration;
    float X;
    float H[h_size];
    const float *W;
    float overlap[overlap_size];
    float buffer[sample_length * 2];
    float wfft[fft_size * 2 >> 1];
    int   ip[fft_size * 2 >> 1];

    void dsp_logic();
};

#endif // EQUALIZER_FILTER_H
