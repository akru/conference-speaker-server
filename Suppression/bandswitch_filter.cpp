#include "bandswitch_filter.h"
#include "fft4g.h"
#include "bandswitch_firs.h"

BandswitchFilter::BandswitchFilter()
    : currentBand(0),
      iteration(0)
{
    Q_ASSERT(sample_rate == 8000);

    xv.resize(BAND_COUNT*FILTER_COUNT);
    for (short i = 0; i < BAND_COUNT*FILTER_COUNT; ++i)
        xv[i].fill(0, sample_length);

}

BandswitchFilter::~BandswitchFilter()
{}

QByteArray BandswitchFilter::process(const QByteArray &sample)
{
    Q_ASSERT(sample.length() == sample_length * sizeof(qint16));

    if (++iteration > BAND_SHIFT_TIME / 32.0)
    {
        currentBand = (currentBand + 1) % BAND_COUNT;
        iteration   = 0;
    }

    int   ip[HS_IP_LENGTH]; ip[0] = 0;
    float wfft[HS_W_LENGTH],
          outVect[FILTER_COUNT + 1][HS_BLOCKL];
    memset(outVect[0], 0, HS_BLOCKL * sizeof(float));
    rdft(HS_BLOCKL, 1, outVect[0], ip, wfft);
    memset(outVect[0], 0, HS_BLOCKL * sizeof(float));

    const short *input_p;
    for (short i = 1; i < FILTER_COUNT + 1; ++i)
    {
        // Apply filter
        input_p = (qint16 *) sample.data();
        for (short j = 0; j < HS_BLOCKL; ++j)
            outVect[i][j] = firProcess(i+currentBand*FILTER_COUNT - 1, *input_p++);
        // FFT
        rdft(HS_BLOCKL, 1, outVect[i], ip, wfft);
        // Sum with
        for (short j = 0; j < HS_BLOCKL; ++j)
            outVect[0][j] += outVect[i][j];
    }
    // IFFT
    rdft(HS_BLOCKL, -1, outVect[0], ip, wfft);
    for (short j = 0; j <= HS_BLOCKL - 1; j++) {
        outVect[0][j] *= 2.0 / HS_BLOCKL;
    }
    // Gen output
    QByteArray out; out.resize(sample.length());
    qint16 *out_p = (qint16 *) out.data();
    for (short j = 0; j < HS_BLOCKL; ++j)
        *out_p++ = outVect[0][j];

    return out;
}

double BandswitchFilter::firProcess(int i, double wave)
{
    // Direct form I FIR filter
    // SV rift
    xv[i].pop_front(); xv[i].push_back(wave);
    // Calc res by params and input
    double result = 0;
    for (short k = 0; k < xv[i].length(); ++k)
        result += xv[i].at(k) * FIR[i][k];
    return result;
}
