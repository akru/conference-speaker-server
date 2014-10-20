#include "ampanalyze_filter.h"

/*
 * Average amplitude analyzer.
 * Formula:
 *   A = (a1 + a2 + ... + aN) / N
 * where A is normalized average amplitude,
 * aN is custom amplutude from sample
 * and N is count of amplitudes.
 *
 * Sample is a array of 16bit signed integers.
 * PCM 16bit sample.
 *
 * This function returns number from 0 to 100.
 * That shows current sample average amplitude.
 */
void AmpAnalyzeFilter::processFilter(float sample[])
{
    // Variable init
    float avgAmp = 0;
    // Sum all amps from sample
    for (short i = 0; i < sample_length; ++i)
        avgAmp += fabs(sample[i]);
    // Divize sum by count and normalize it
    avgAmp = avgAmp / sample_length;
    // Return average amplitude in percents
    amp = avgAmp * 100;
}
