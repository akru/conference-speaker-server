#include "bandswitch_filter.h"
#include "fft4g.h"

BandswitchFilter::BandswitchFilter(quint32 sample_rate)
    : currentBand(0),
      iteration(0)
{
    Q_ASSERT(sample_rate == 8000);
    filter.resize(BAND_COUNT);

    BiquadParams biq;
    biq.Q = 2; // Because ? I don't know but may be it's working
    for (short i = 0; i < BAND_COUNT; ++i)
        for (short j = 0; j < FILTER_COUNT; ++j)
        {
            biq.freq = BAND_FREQ[i][j];
            BiquadCalcBandpass(&biq);
            filter[i].append(biq);
        }
}

BandswitchFilter::~BandswitchFilter()
{}

QByteArray BandswitchFilter::process(const QByteArray &sample)
{
    Q_ASSERT(sample.length() == 512);
    if (++iteration > BAND_SHIFT_TIME)
    {
        currentBand = (currentBand + 1) % BAND_COUNT;
        iteration   = 0;
    }

    int   ip[HS_IP_LENGTH]; ip[0] = 0;
    float wfft[HS_W_LENGTH],
          outVect[FILTER_COUNT + 1][HS_BLOCKL];
    memset(outVect[0], 0, HS_BLOCKL * sizeof(float));

    const short *input_p;
    for (short i = 1; i < FILTER_COUNT + 1; ++i)
    {
        // Apply filter
        input_p = (qint16 *) sample.data();
        for (short j = 0; j < HS_BLOCKL; ++j)
            outVect[i][j] = BiquadProcess(&filter[currentBand][i - 1], *input_p++);
        // FFT
        rdft(HS_BLOCKL, 1, outVect[i], ip, wfft);
        // Sum with
        for (short j = 0; j < HS_BLOCKL; ++j)
            outVect[0][j] += outVect[i][j];
    }
    // IFFT
    rdft(HS_BLOCKL, -1, outVect[0], ip, wfft);
    // Gen output
    QByteArray out; out.resize(sample.length());
    qint16 *out_p = (qint16 *) out.data();
    for (short j = 0; j < HS_BLOCKL; ++j)
        *out_p++ = outVect[0][j];

    return out;
}
