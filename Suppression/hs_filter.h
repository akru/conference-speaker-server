#ifndef HS_FILTER_H
#define HS_FILTER_H

#include "filter.h"
#include "equalizer_filter.h"
#include <cmath>


namespace HS {
// Static constants
static const short analyze_length = Filter::sample_length;
static const short fft_input_length = analyze_length * 2;
// Must be at least ceil(2 + sqrt(HS_BLOCKL/2))
static const short ip_length = (fft_input_length * 2 >> 1);
static const short w_length = (fft_input_length * 2 >> 1);
static const short dft_buffer_count = 5;
static const int   dft_buffer_len = dft_buffer_count * analyze_length;
static const short freq_count_max = 20;
static const float index_to_hz = Filter::sample_rate / 2.0f / analyze_length;
static const float index_to_rad = 2.0f * M_PI * Filter::sample_rate / analyze_length;
static const float hz_to_index = analyze_length * 2.0f / Filter::sample_rate;
static const float rad_to_index = analyze_length / 2.0f / M_PI / Filter::sample_rate;
static const short minimal_freq = 50;
static const short maximal_freq = 11025;
static const float filter_step = 0.1f;
}

#define HS_DEBUG

class HSFilter : public Filter
{
public:
    HSFilter(EqualizerFilter *eq);
    ~HSFilter();

    void processFilter(float sample[]);
    QString name() { return "Howling suppression"; }
    void reloadSettings();

    inline void setTH(float PAPR_TH_, float PHPR_TH_,
                      float PNPR_TH_, float IMSD_TH_)
    {
        PAPR_TH = PAPR_TH_;
        PHPR_TH = PHPR_TH_;
        PNPR_TH = PNPR_TH_;
        IMSD_TH = IMSD_TH_;
    }

private:
    // Equalizer instance
    EqualizerFilter *eq;
    // Analyzer data
    int   ip[HS::ip_length];
    float wfft[HS::w_length];
    float dft_buf[HS::dft_buffer_len];
    // Trasholds
    float PAPR_TH, PHPR_TH, PNPR_TH, IMSD_TH;

    short analyze_howling(short howling_freq[], const float input[]);
    void  evaluatePAPR(const float input[], float papr[]);
    void  evaluatePHPR(const float input[], float phpr[]);
    void  evaluatePNPR(const float input[], float pnpr[]);
    void  evaluateIMSD(const float input[], float imsd[]);
};

#endif // HS_FILTER_H
