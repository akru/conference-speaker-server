#ifndef PROCESSING_H
#define PROCESSING_H

#include <QByteArray>
#include "accbuffer.hpp"

class Filter;

class Processing
{
public:
    Processing();
    ~Processing();

    /*
     * Insert packet for processing
     * @param packet Is a PCM int16
     */
    void insert(const QByteArray &packet);
    /*
     * Take sample from processing cycle
     * returns: PCM int16 sample with length = Filter::sample_length
     */
    QByteArray take();
    /*
     * Local volume scaling.
     * @param v Is a relative speaker volume: [0,1]
     */
    inline void setVolume(float v)
    {
        Q_ASSERT(v <= 1 && v >= 0);
        gScaler = v;
    }
    /*
     * Average last sample amplitude getter.
     */
    inline uint getAmp() const
    { return avgAmp; }
    /*
     * Reload settings for multiple filters.
     */
    void reloadSettings();
    /*
     * Simple sample mixer.
     * @param s1 Is a int16 first input sample
     * @param s2 Is a int16 second input sample
     * returns: sample that preset mix of two samples
     */
    static QByteArray mix(const QByteArray &s1, const QByteArray &s2);

protected:
    float             gScaler;
    AccBuffer<qint16> acc;
    QList<Filter *>   filters;
    uint              avgAmp;

    // Multiple filters manipulation
    /*
     * Apply multiple filters to raw sample.
     * @param raw_sample Is a sample to processing
     */
    void applyFilters(QByteArray &raw_sample);

    // Sample conversions
    void fromPCM(qint16 pcm[], float sample[]);
    static void toPCM(float sample[], qint16 pcm[]);
};

#endif // PROCESSING_H
