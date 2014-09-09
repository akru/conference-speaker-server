#ifndef EQUALIZER_FILTER_H
#define EQUALIZER_FILTER_H

#include "../Speaker/filter.h"

class EqualizerFilter : public Filter
{
public:
    EqualizerFilter(float X,  //multiplier
            const float *H, //The freq. magnitude scalers filter
            const float *W  //The windowing function
            );
    ~EqualizerFilter();

    void process(float sample[]);
    QString name() { return "Equalizer"; }

    static const int window_size  = sample_length;
    static const int R            = window_size / 2;
    static const int overlap_size = window_size - R;
    static const int fft_size     = sample_length * 2;

private:
    bool first_iteration;
    float X;  //multiplier
    const float *H; //The freq. magnitude scalers filter
    const float *W; //The windowing function
    float *overlap;
    float *buffer;

    void dsp_logic();
};

#endif // EQUALIZER_FILTER_H
