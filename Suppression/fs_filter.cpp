#ifndef FS_FILTER_CPP
#define FS_FILTER_CPP

#include "defines.h"
#include "fft4g.h"
#include "fs_filter.h"
#include <QDebug>
#include "windows_private.h"

FSFilter::FSFilter()
    : iteration(0), currentState(false)
{
    Q_ASSERT(sample_rate == 8000);
}

FSFilter::~FSFilter()
{}

QByteArray FSFilter::process(const QByteArray &sample)
{
    Q_ASSERT(sample.length() == sample_length * sizeof(qint16));

    if (++iteration > TONE_SHIFT_TIME / 32.0)
    {
        currentState = !currentState;
        iteration    = 0;
    }

    int   ip[HS_IP_LENGTH]; ip[0] = 0;
    float wfft[HS_W_LENGTH],
          outVect[HS_BLOCKL];

    memset(outVect, 0, HS_BLOCKL * sizeof(float));
    rdft(HS_BLOCKL, 1, outVect, ip, wfft);
    memset(outVect, 0, HS_BLOCKL * sizeof(float));

    qint16 *sam_p = (qint16 *) sample.data();
    for (short i = 0; i < HS_BLOCKL; ++i)
        outVect[i] = (float)sam_p[i];

    if (currentState)
    {
            // FFT
            rdft(HS_BLOCKL, 1, outVect, ip, wfft);

            // Add shift
            for (short i = 0; i < HS_BLOCKL - 2; ++i)
            {
                qDebug() << i << "=" << outVect[i];
                outVect[i] = outVect[i+2];
            }
            outVect[HS_BLOCKL - 1] = 0; outVect[HS_BLOCKL] = 0;

            // IFFT
            rdft(HS_BLOCKL, -1, outVect, ip, wfft);
            for (short i = 0; i < HS_BLOCKL; i++)
            {
                outVect[i] *= 2.0 / HS_BLOCKL;
                qDebug() << i << "=" << outVect[i];
            }
    }

    QByteArray out; out.resize(sample.length());
    qint16 *out_p = (qint16 *) out.data();
    for (short i = 0; i < HS_BLOCKL; ++i)
        *out_p++ = outVect[i];

    return out;

}

#endif // FS_FILTER_CPP
